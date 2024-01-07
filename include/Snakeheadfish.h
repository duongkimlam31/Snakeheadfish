#ifndef SNAKEHEADFISH_H
#define SNAKEHEADFISH_H

#include "polyglot/polybook.cc"
#include "chess.hpp"
#include "Fathom/fathom.h"
#include "TranspositionTable.h"
#include "Evaluator.h"
#include <unordered_map>
#include <random>
#include <iterator>
#include <thread>
#include <mutex>
#include <vector>
#include <iostream>
#include <unordered_set>

// Declare global variables
#define ASPIRATION_WINDOW 150

#define UPPERBOUND 1
#define EXACT 0
#define LOWERBOUND -1

#define CutNode 1
#define PVNode 0
#define AllNode -1

#define OPP_NODE(node_type)     (-(node_type))

const int num_processor = std::thread::hardware_concurrency();
int futility_margin[4] = {0, 600, 1600, 2800};

int mvv_lva[7][7] = {
  {15, 14, 13, 12, 11, 10, 0},
  {25, 24, 23, 22, 21, 20, 0},
  {35, 34, 33, 32, 31, 30, 0},
  {45, 44, 43, 42, 41, 40, 0},
  {55, 54, 53, 52, 51, 50, 0},
  {0 ,  0,  0,  0,  0,  0, 0},
  {15 ,  0,  0,  0,  0,  0, 0}
};

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

class Snakeheadfish{
  private:
    int search_depth;
    int nnue_on;
    chess::Move next_move;
    int mg_table[12][64] = {};
    int eg_table[12][64] = {};
    std::vector<std::thread> threads;
    unsigned int ****history_tables;
    std::vector<std::unordered_map<int, std::vector<chess::Move>>> killer_moves_tables;
    PolyBookReader *polyglot;
    int thread_num;
    TranspositionTable *transposition_table;
    std::mutex *entry_locks;
    Evaluator *evaluator;

  public:
    Snakeheadfish(char *book_path, int search_depth=6, int nnue_on = 1){
      this->search_depth = search_depth;
      this->thread_num = num_processor;
      history_tables = (unsigned int ****) malloc(this->thread_num * sizeof(unsigned int ***));
      for (int i = 0; i < this->thread_num; ++i){
        history_tables[i] = (unsigned int ***) malloc(2 * sizeof(unsigned int**));
        for (int j = 0; j < 2; ++j){
          history_tables[i][j] = (unsigned int **) malloc(64 * sizeof(unsigned int*));
          for (int k = 0; k < 64; ++k){
            history_tables[i][j][k] = (unsigned int *) malloc(64 * sizeof(unsigned int));
          }
        }
      }

      init_history_tables();
      tb_init("../3-4-5_pieces_Syzygy");
      polyglot = new PolyBookReader(book_path);
      threads.resize(thread_num);
      killer_moves_tables.resize(thread_num);
      this->transposition_table = new TranspositionTable();
      this->entry_locks = new std::mutex[this->transposition_table->get_size()];
      this->evaluator = new Evaluator();
      this->nnue_on = nnue_on;
    }
    
     ~Snakeheadfish(){
      for (int i = 0; i < this->thread_num; ++i){
        for (int j = 0; j < 2; ++j){
          for (int k = 0; k < 64; ++k){
            free(history_tables[i][j][k]);
          }
          free(history_tables[i][j]);
        }
        free(history_tables[i]);
      }
      free(history_tables);
      delete this->polyglot;
      delete this->transposition_table;
      delete this->evaluator;
      delete[] this->entry_locks;
    }

    void init_history_tables(){
      for(int t = 0; t < thread_num; ++t){
        for (int i = 0; i < 2; ++i){
          for (int j = 0; j < 64; ++j){
            for (int k = 0; k < 64; ++k){
              this->history_tables[t][i][j][k] = 0;
            }
          }
        }
      }
    }

    void setSearchDepth(int depth){
      this->search_depth = depth;
    }
    

    chess::Move get_next_move(){
      return this->next_move;
    }

    int to_move(const chess::Board &chessboard){
      if (chessboard.sideToMove() == chess::Color::WHITE){
        return 1;
      }
      return 0;
    }

    chess::Movelist actions(const chess::Board &chessboard, int thread_id, int depth, int ply , const chess::Movelist &legal_moves , const chess::Move &hash_move){
      chess::Movelist legal_captures;
      chess::Movelist legal_non_captures;
      for (const auto &move : legal_moves){
        if (!chessboard.isCapture(move)){
          legal_non_captures.add(move);
        }
        else{
          legal_captures.add(move);
        }
      }
      order_moves(chessboard, legal_captures, legal_non_captures, thread_id, depth);
      chess::Movelist ordered_moves;
      std::unordered_set<std::string> move_set;
      if (hash_move != chess::Move::NO_MOVE && move_set.find(chess::uci::moveToUci(hash_move)) == move_set.end()){
        ordered_moves.add(hash_move);
        move_set.insert(chess::uci::moveToUci(hash_move));
      }
      for (const auto &capture : legal_captures){
        if (move_set.find(chess::uci::moveToUci(capture)) == move_set.end()){
          ordered_moves.add(capture);
          move_set.insert(chess::uci::moveToUci(capture));
        }
      }
      if (this->killer_moves_tables[thread_id].find(ply) != this->killer_moves_tables[thread_id].end()){
        chess::Move killer_move_1 = this->killer_moves_tables[thread_id][ply].at(0);
        chess::Move killer_move_2 = this->killer_moves_tables[thread_id][ply].at(1);
        for (const auto &non_capture : legal_non_captures){
          if (non_capture == killer_move_1 && move_set.find(chess::uci::moveToUci(killer_move_1)) == move_set.end()){
            ordered_moves.add(killer_move_1);
            move_set.insert(chess::uci::moveToUci(killer_move_1));
            break;
          }
        }
        if (killer_move_2 != chess::Move::NO_MOVE){
          for (const auto &non_capture : legal_non_captures){
            if (non_capture == killer_move_2 && move_set.find(chess::uci::moveToUci(killer_move_2)) == move_set.end()){
              ordered_moves.add(killer_move_2);
              move_set.insert(chess::uci::moveToUci(killer_move_2));
              break;
            }
          }
        }
      }
      for (const auto &non_capture : legal_non_captures){
        if (move_set.find(chess::uci::moveToUci(non_capture)) == move_set.end()){
          ordered_moves.add(non_capture);
          move_set.insert(chess::uci::moveToUci(non_capture));
        }
      }
      if (ordered_moves.size() != legal_moves.size()){
        std::cout << "Wrong\n";
      }
      return ordered_moves;
    }

    bool is_cutoff(const chess::Board &chessboard, const chess::Movelist &legal_moves){
      bool checkmate = (legal_moves.empty() && chessboard.inCheck());
      bool stalemate = (legal_moves.empty() && !chessboard.inCheck());
      if (checkmate || stalemate || chessboard.isInsufficientMaterial() || chessboard.isRepetition(1) || chessboard.isHalfMoveDraw()){
        return true;
      }
      return false;
    }

    void order_moves(const chess::Board &chessboard, chess::Movelist &legal_captures, chess::Movelist &legal_non_captures, int thread_id, int depth){
      mvv_lva_heuristic(chessboard, legal_captures);
      history_heuristic(chessboard, legal_non_captures, thread_id, depth);
    }

    void mvv_lva_heuristic(const chess::Board &chessboard, chess::Movelist &legal_captures){
      std::unordered_map<chess::PieceType, int>  piece_dictionary = {{chess::PieceType::PAWN, 0}, {chess::PieceType::KNIGHT, 1}, {chess::PieceType::BISHOP, 2}, {chess::PieceType::ROOK, 3}, {chess::PieceType::QUEEN, 4}, {chess::PieceType::KING, 5}, {chess::PieceType::NONE, 6}};
      std::stable_sort(legal_captures.begin(), legal_captures.end(), [&](const chess::Move &capture_1, const chess::Move &capture_2){
        int victim_1 = -1;
        int victim_2 = -1;
        int aggressor_1 = -1;
        int aggressor_2 = -1;

        chess::Square victim_1_square = capture_1.to();
        chess::Piece victim_1_piece = chessboard.at(victim_1_square);
        chess::PieceType victim_1_piece_type = chess::utils::typeOfPiece(victim_1_piece);
        victim_1 = piece_dictionary[victim_1_piece_type];

        chess::Square aggressor_1_square = capture_1.from();
        chess::Piece aggressor_1_piece = chessboard.at(aggressor_1_square);
        chess::PieceType aggressor_1_piece_type = chess::utils::typeOfPiece(aggressor_1_piece);
        aggressor_1 = piece_dictionary[aggressor_1_piece_type];

        chess::Square victim_2_square = capture_2.to();
        chess::Piece victim_2_piece = chessboard.at(victim_2_square);
        chess::PieceType victim_2_piece_type = chess::utils::typeOfPiece(victim_2_piece);
        victim_2 = piece_dictionary[victim_2_piece_type];

        chess::Square aggressor_2_square = capture_2.from();
        chess::Piece aggressor_2_piece = chessboard.at(aggressor_2_square);
        chess::PieceType aggressor_2_piece_type = chess::utils::typeOfPiece(aggressor_2_piece);
        aggressor_2 = piece_dictionary[aggressor_2_piece_type];
        
        return (mvv_lva[victim_1][aggressor_1]) > (mvv_lva[victim_2][aggressor_2]);
      });
    }

    void history_heuristic(const chess::Board &chessboard, chess::Movelist &legal_non_captures, int thread_id, int depth){
      std::stable_sort(legal_non_captures.begin(), legal_non_captures.end(), [&](const chess::Move &non_capture_1, const chess::Move &non_capture_2){
        return (this->history_tables[thread_id][to_move(chessboard)][non_capture_1.from()][non_capture_1.to()]) > (this->history_tables[thread_id][to_move(chessboard)][non_capture_2.from()][non_capture_2.to()]);
      });
    }

    float quiescence_search(chess::Board &chessboard, float alpha, float beta){
      chess::Movelist legal_moves;
      chess::Movelist legal_captures;
      chess::movegen::legalmoves(legal_moves, chessboard);
      if (chessboard.isInsufficientMaterial() || chessboard.isRepetition(1) || chessboard.isHalfMoveDraw()){
        if (nnue_on){
          return evaluator->nnue_evaluation_function(chessboard, legal_moves);
        }
        return evaluator->evaluation_function(chessboard, legal_moves);
      }
      float stand_pat;
      if (!nnue_on){
        stand_pat = evaluator->evaluation_function(chessboard, legal_moves);
      }
      else{
        stand_pat = evaluator->nnue_evaluation_function(chessboard, legal_moves);
      }
      for (const auto &move : legal_moves){
        if (chessboard.isCapture(move)){
          legal_captures.add(move);
        }
      }
      int material_balance = evaluator->material_balance(chessboard);
      if (chessboard.sideToMove() == chess::Color::BLACK){
        material_balance *= -1;
      }
      int delta = 600;
      bool delta_prune = false;
      float delta_val = delta + stand_pat;
      if (delta_val <= alpha && !chessboard.inCheck()){
        delta_prune = true;
      }
      if (alpha < stand_pat){
        alpha = stand_pat;
        if (stand_pat >= beta){
          return stand_pat;
        }
      }
      mvv_lva_heuristic(chessboard, legal_captures);
      float best_val = stand_pat;
      for (const auto &capture : legal_captures){
        // Delta pruning
        int new_material_balance = 1;
        if (chessboard.sideToMove() == chess::Color::BLACK){
          new_material_balance = -1;
        }
        chessboard.makeMove(capture);
        new_material_balance *= evaluator->material_balance(chessboard);
        int material_gain = new_material_balance-material_balance;
        if (delta_prune && !chessboard.inCheck() && delta_val+material_gain <= alpha){
          chessboard.unmakeMove(capture);
          continue;
        }
        float val = -quiescence_search(chessboard, -beta, -alpha);
        chessboard.unmakeMove(capture);
        if (val > best_val){
          best_val = val;
          if (val > alpha){
            alpha = val;
            if (val >= beta){
              break;
            }
          }
        }
      }
      return best_val;
    }

    float alpha_beta_negamax(chess::Board &chessboard, float alpha, float beta, int depth, int thread_id, bool verify, int node_type, bool allow_null_move = true){
      int ply = chessboard.getPly();
      float original_alpha = alpha;
      uint64_t zobrist_hash = chessboard.hash();
      int lock_index = this->transposition_table->hash_function(zobrist_hash);
      float val;
      chess::Move hash_move = chess::Move::NO_MOVE;
      tt_entry_t entry;
      bool valid_entry = false;
      chess::Movelist legal_moves;
      chess::movegen::legalmoves(legal_moves, chessboard);
      float static_eval;
      bool null_move_fail_high = false;
      if (nnue_on){
        static_eval = evaluator->nnue_evaluation_function(chessboard, legal_moves);
      }
      else{
        static_eval = evaluator->evaluation_function(chessboard, legal_moves);
      }
      if (is_cutoff(chessboard, legal_moves)){
        if (nnue_on){
          return evaluator->nnue_evaluation_function(chessboard, legal_moves);
        }
        return evaluator->evaluation_function(chessboard, legal_moves);
      }
      if (depth <= 0){
        return quiescence_search(chessboard, alpha, beta);
      }
      entry_locks[lock_index].lock();
      valid_entry = this->transposition_table->is_valid_key(zobrist_hash);
      if (valid_entry){
        entry = this->transposition_table->probe_table(zobrist_hash);
        hash_move = entry.best_move;
        static_eval = entry.best_value;
        if (entry.depth >= depth && node_type != PVNode){
          if (entry.flag == EXACT){
            entry_locks[lock_index].unlock();
            return entry.best_value;
          }
          else if (entry.flag == LOWERBOUND){
            alpha = std::max(alpha, entry.best_value);
          }
          else if (entry.flag == UPPERBOUND){
            beta = std::min(beta, entry.best_value);
          }
          if (alpha >= beta){
            entry_locks[lock_index].unlock();
            return entry.best_value;
          }
        }
      }
      entry_locks[lock_index].unlock();
      // verified null-move pruning
      if (!chessboard.inCheck() && (depth > 1 || !verify) && node_type != PVNode && allow_null_move){
        chessboard.makeNullMove();
        val = -alpha_beta_negamax(chessboard, -beta, -beta+1, depth-4, thread_id, verify, OPP_NODE(node_type), false);
        chessboard.unmakeNullMove();
        if (val >= beta){
          if (verify){
            depth--;
            verify = false;
            null_move_fail_high = true;
          }
          else{
            return val;
          }
        }
      }
      // Multi-Probcut
      if (depth > 3 && node_type != PVNode){
        float bound;
        float a;
        float b;
        float sigma;
        float percentile;
        if (!evaluator->is_end_game(chessboard)){
          a = 1.13;
          b = 22.5;
          sigma = 600;
          percentile = 1;
        }
        else{
          a = 1.1;
          b = 15;
          sigma = 500;
          percentile = 1.4;
        }
        int shallow_depth;
        if (depth <= 8){
          shallow_depth = ceil(depth/2.0);
        }
        else{
          shallow_depth = depth-4;
        }
        bound = std::round((percentile * sigma + beta - b) / a);
        val = alpha_beta_negamax(chessboard, bound-1, bound, shallow_depth, thread_id, verify, node_type, allow_null_move);
        if (val >= bound){
          return val;
        }
        bound = std::round((-percentile * sigma + alpha - b) / a);
        val = alpha_beta_negamax(chessboard, bound, bound+1, shallow_depth, thread_id, verify, node_type, allow_null_move);
        if (val <= bound){
          return val;
        }
      }
      // Deep futility pruning
      int fmax, fprune, fscore, score, material_balance;
      material_balance = evaluator->material_balance(chessboard);
      fmax = -CHECKMATE_VAL;
      fprune = 0;
      if (chessboard.sideToMove() == chess::Color::BLACK){
        material_balance *= -1;
      }
      fscore = static_eval + futility_margin[depth];
      if (!chessboard.inCheck() && depth == 3 && fscore <= alpha && node_type != PVNode){
        int white_num_pieces = 0;
        int black_num_pieces = 0;
        for(int sq = 0; sq < chess::constants::MAX_SQ; ++sq){
          chess::Piece piece = chessboard.at(chess::Square(sq));
          if (chessboard.color(piece) == chess::Color::WHITE){
            white_num_pieces++;
          }
          else if (chessboard.color(piece) == chess::Color::BLACK){
            black_num_pieces++;
          }
        }
        if (chessboard.sideToMove() == chess::Color::WHITE && black_num_pieces - white_num_pieces > 3){
          depth = 2;
        }
        else if (chessboard.sideToMove() == chess::Color::BLACK && white_num_pieces - black_num_pieces > 3){
          depth = 2;
        }
      }
      fscore = static_eval + futility_margin[depth];
      if (!chessboard.inCheck() && (depth == 2 || depth == 1) && fscore <= alpha && node_type != PVNode){
        fprune = 1;
        fmax = fscore;
      }
      // Check extension
      if (chessboard.inCheck()){
        depth++;
      }
      // One reply extension
      else if (legal_moves.size() == 1){
        depth++;
      }
      float tmp_alpha = alpha;
      chess::Movelist ordered_moves;
      ordered_moves = actions(chessboard, thread_id, depth, ply, legal_moves, hash_move);
      goto search;
      search:
        int moves_searched = 0;
        chess::Move best_move = chess::Move::NO_MOVE;
        float best_val = NO_VAL;
        int r = 1;
        if (node_type == CutNode){
          r += 2;
        }
        int child_node_type = AllNode;
        if (node_type != CutNode){
          child_node_type = CutNode;
        }
        for (const auto &move : ordered_moves){
          chess::Board new_board = chessboard;
          new_board.makeMove(move);
          if (moves_searched == 0 && node_type == PVNode){
            val = -alpha_beta_negamax(new_board, -beta, -alpha, depth-1, thread_id, verify, PVNode);
          }
          else{
            bool killer_move = false;
            if (this->killer_moves_tables[thread_id].find(ply) != this->killer_moves_tables[thread_id].end()){
              killer_move = (this->killer_moves_tables[thread_id][ply].at(0) == move || this->killer_moves_tables[thread_id][ply].at(1) == move);
            }
            // Deep futility pruning
            int new_material_balance = evaluator->material_balance(new_board);
            if (chessboard.sideToMove() == chess::Color::BLACK){
              new_material_balance *= -1;
            }
            int material_gain = new_material_balance-material_balance;
            if (fprune && (fmax + material_gain) <= alpha && !(new_board.inCheck()) && !killer_move){
              moves_searched++;
              continue;
            }
            // Late move reductions
            bool allow_late_move_reductions = (!chessboard.inCheck() && !new_board.inCheck() && !chessboard.isCapture(move) && move.typeOf() != chess::Move::PROMOTION && node_type != PVNode && !killer_move);
            if (moves_searched > 1 && depth >= 2 && allow_late_move_reductions){
              val = -alpha_beta_negamax(new_board, -(alpha+1), -alpha, depth-1-r, thread_id, verify, child_node_type);
            }
            else{
              val = alpha + 1; //Ennsure that it starts PV Search if lmr conditions don't meet
            }
            // PV search
            if (val > alpha){
              val = -alpha_beta_negamax(new_board, -(alpha+1), -alpha, depth-1, thread_id, verify, child_node_type);
              if (val > alpha && val < beta){
                val = -alpha_beta_negamax(new_board, -beta, -alpha, depth-1, thread_id, verify, PVNode);     
              }
            }
          }
          if (val > best_val){
            best_val = val;
            if (val > alpha){
              best_move = move;
              alpha = val;
              if (val >= beta){
                if (!chessboard.isCapture(move)){
                  if (this->killer_moves_tables[thread_id].find(ply) == this->killer_moves_tables[thread_id].end()){
                    std::vector<chess::Move> moves = {chess::Move::NO_MOVE, chess::Move::NO_MOVE};
                    moves.at(0) = move;
                    this->killer_moves_tables[thread_id][ply] = moves;
                  }
                  else{
                    if (move != this->killer_moves_tables[thread_id][ply].at(0)){
                      this->killer_moves_tables[thread_id][ply].at(1) = this->killer_moves_tables[thread_id][ply].at(0);
                      this->killer_moves_tables[thread_id][ply].at(0) = move;
                    }
                  }
                }
                break;
              }
            }
          }
          moves_searched++;
        }
        if (null_move_fail_high && val < beta){
          depth++;
          null_move_fail_high = false;
          verify = true;
          alpha = tmp_alpha;
          goto search;
        }
        if (!chessboard.isCapture(best_move)){
          this->history_tables[thread_id][to_move(chessboard)][best_move.from()][best_move.to()] += depth*depth;
        }
        if (best_val == -CHECKMATE_VAL && best_move == chess::Move::NO_MOVE){
          best_move = ordered_moves[0];
        }
        if ((valid_entry && entry.depth <= depth) || !valid_entry){
          tt_entry_t new_entry;
          if (best_val <= original_alpha){
            new_entry.flag = UPPERBOUND;
          }
          else if (best_val >= beta){
            new_entry.flag = LOWERBOUND;
          }
          else{
            new_entry.flag = EXACT;
          }
          new_entry.depth = depth;
          new_entry.best_value = best_val;
          new_entry.best_move = best_move;
          entry_locks[lock_index].lock();
          this->transposition_table->update_table(zobrist_hash, new_entry);
          entry_locks[lock_index].unlock();
        }
        return best_val;
    }

    float lazy_smp(chess::Board chessboard){
      init_history_tables();
      for (int i = 0; i < this->thread_num;++i){
        this->killer_moves_tables[i].clear();
      }
      this->next_move = chess::Move::NO_MOVE;
      chess::Movelist opening_moves;
      uint64_t polykey = polyglot->PolyKeyFromBoard(chessboard);
      opening_moves = polyglot->listBookMoves(polykey, chessboard);
      if (opening_moves.size() > 0){
        this->next_move = *select_randomly(opening_moves.begin(), opening_moves.end());
        return NO_VAL;
      }
      chess::Movelist legal_moves;
      chess::movegen::legalmoves(legal_moves, chessboard);
      if (legal_moves.size() == 1){
        this->next_move = legal_moves[0];
        return NO_VAL;
      }
      const auto total_pieces = chess::builtin::popcount(chessboard.occ());
      if (total_pieces <= 5){
        struct pos pos0;
        struct pos *pos = &pos0;
        std::string fen = chessboard.getFen();
        const char* c_fen = fen.c_str();
        if (!parse_FEN(pos, c_fen)){
          fprintf(stderr, "error: unable to parse FEN string \"%s\"\n", c_fen);
          exit(EXIT_FAILURE);
        }
        if (tb_pop_count(pos->white | pos->black) > TB_LARGEST)
        {
            fprintf(stderr, "error: unable to probe tablebase; FEN string \"%s\" "
                "has too many pieces (max=%u)\n", c_fen, TB_LARGEST);
            exit(EXIT_FAILURE);
        }
        unsigned move = tb_probe_root(pos->white, pos->black, pos->kings,
            pos->queens, pos->rooks, pos->bishops, pos->knights, pos->pawns,
            pos->rule50, pos->castling, pos->ep, pos->turn, NULL);
        if (move == TB_RESULT_FAILED)
        {
          printf("{TB probe failed}\n");
        }
        char str[32];
        move_to_str(pos, move, str);
        this->next_move = chess::uci::parseSan(chessboard, str);
        return NO_VAL;
      }
      std::vector<chess::Board> chessboards;
      for (int i = 0; i < this->thread_num; ++i){
        chess::Board tmp_chessboard = chessboard;
        chessboards.push_back(tmp_chessboard);
      }
      threads[0] = std::thread(&Snakeheadfish::iterative_deepening_search, this, std::ref(chessboards.at(0)), 0, 1);
      for (int id = 1; id < this->thread_num/2; ++id){
        threads[id] = std::thread(&Snakeheadfish::iterative_deepening_search, this, std::ref(chessboards.at(id)), id, this->search_depth-1);
      }
      for (int id = this->thread_num/2; id < this->thread_num; ++id){
        threads[id] = std::thread(&Snakeheadfish::iterative_deepening_search, this, std::ref(chessboards.at(id)), id, this->search_depth);
      }
      for (int id = 0; id < this->thread_num; ++id){
        threads[id].join();
      }
      uint64_t zobrist_hash = chessboard.hash();
      tt_entry_t entry = this->transposition_table->probe_table(zobrist_hash);
      this->next_move = entry.best_move;
      return entry.best_value;
    }

    void iterative_deepening_search(chess::Board &chessboard, int id, int search_depth){
      int alpha = -CHECKMATE_VAL;
      int beta = CHECKMATE_VAL;
      int best_value = NO_VAL;
      tt_entry_t entry;
      std::string fen = chessboard.getFen();
      const char* c_fen = fen.c_str();
      uint64_t zobrist_hash = chessboard.hash();
      int cutoff_depth = 1;
      while (cutoff_depth <= search_depth){
        alpha_beta_negamax(chessboard, alpha, beta, cutoff_depth, id, true, PVNode);
        int lock_index = this->transposition_table->hash_function(zobrist_hash);
        entry_locks[lock_index].lock();
        entry = this->transposition_table->probe_table(zobrist_hash);
        best_value = entry.best_value;
        entry_locks[lock_index].unlock();
        if (std::abs(best_value) == CHECKMATE_VAL){
          break;
        }
        if (best_value <= alpha){
          alpha = -CHECKMATE_VAL;
          continue;
        }
        if (best_value >= beta){
          beta = CHECKMATE_VAL;
          continue;
        }
        alpha = best_value - ASPIRATION_WINDOW;
        beta = best_value + ASPIRATION_WINDOW;
        cutoff_depth++;
      }
    }
};

#endif