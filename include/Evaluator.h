#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "piece_square_tables.h"
#include "chess.hpp"
#include <unordered_set>
#include <string>
#include "nnue.h"
#define CHECKMATE_VAL 999999

class Evaluator{
  public:
    Evaluator(){
      nnue_init("nn-eba324f53044.nnue");
    }
    int crowded_pawns_evaluation(const chess::Board &chessboard){
      std::unordered_set<int> white_team_crowded_files;
      std::unordered_set<int> black_team_crowded_files;
      int num_white_team_crowded_pawns = 0;
      int num_black_team_crowded_pawns = 0;
      for(int sq = 0; sq < chess::constants::MAX_SQ; ++sq){
        chess::Piece piece = chessboard.at(chess::Square(sq));
        chess::PieceType piece_type = chess::utils::typeOfPiece(piece);
        if (piece_type == chess::PieceType::PAWN){
          int file = sq & 7;
          if (chessboard.color(piece) == chess::Color::WHITE){
            if (white_team_crowded_files.find(file) == white_team_crowded_files.end()){
              white_team_crowded_files.insert(file);
            }
            else{
              num_white_team_crowded_pawns++;
            }
          }
          else if (chessboard.color(piece) == chess::Color::BLACK){
            if (black_team_crowded_files.find(file) == black_team_crowded_files.end()){
              black_team_crowded_files.insert(file);
            }
            else{
              num_black_team_crowded_pawns++;
            }
          }
        }
      }
      return -num_white_team_crowded_pawns + num_black_team_crowded_pawns;
    }

    int passed_pawns_evaluation(const chess::Board &chessboard){
      int white_team_passed_pawns_val = 0;
      int black_team_passed_pawns_val = 0;
      for(int sq = 0; sq < chess::constants::MAX_SQ; ++sq){
        chess::Piece piece = chessboard.at(chess::Square(sq));
        chess::PieceType piece_type = chess::utils::typeOfPiece(piece);
        if (piece_type == chess::PieceType::PAWN){
          int file = sq & 7;
          int rank = sq >> 3;
          if (chessboard.color(piece) == chess::Color::WHITE){
            while (rank + 1 <= 7){
              chess::Square next_square = chess::utils::makeSquare(chess::Rank(++rank), chess::File(file));
              chess::Piece next_piece = chessboard.at(next_square);
              chess::PieceType next_piece_type = chess::utils::typeOfPiece(next_piece);
              if (chessboard.color(next_piece) == chess::Color::BLACK && next_piece_type == chess::PieceType::PAWN){
                break;
              }
            }
            if (rank == 7){
              white_team_passed_pawns_val += rank;
            }
          }
          else if (chessboard.color(piece) == chess::Color::BLACK){
            while (rank - 1 >= 0){
              chess::Square next_square = chess::utils::makeSquare(chess::Rank(--rank), chess::File(file));
              chess::Piece next_piece = chessboard.at(next_square);
              chess::PieceType next_piece_type = chess::utils::typeOfPiece(next_piece);
              if (chessboard.color(next_piece) == chess::Color::WHITE && next_piece_type == chess::PieceType::PAWN){
                break;
              }
            }
            if (rank == 0){
              black_team_passed_pawns_val += (7-rank);
            }
          }
        }
      }
      return white_team_passed_pawns_val + -black_team_passed_pawns_val;
    }

    // int isolated_pawns_evaluation(chess::Board chessboard){
    //   std::unordered_set<int> white_team_pawn_files;
    //   std::unordered_set<int> black_team_pawn_files;
    //   int white_pawns[8];
    //   int black_pawns[8];
    //   for(int sq = 0; sq < chess::constants::MAX_SQ; ++sq){
    //     chess::Piece piece = chessboard.at(chess::Square(sq));
    //     chess::PieceType piece_type = chess::utils::typeOfPiece(piece);
    //     if (piece_type == chess::PieceType::PAWN){
    //       if (chessboard.color(piece) == chess::Color::WHITE){
    //         white_pawns
    //       }
    //       else if (chessboard.color(piece) == chess::Color::BLACK){

    //       }
    //     }
    //   }
    //   chess::SquareSet white_pawns = chessboard.pieces(chess::PAWN, chess::WHITE);
    //   chess::SquareSet black_pawns = chessboard.pieces(chess::PAWN, chess::BLACK);
    //   int num_white_team_isolated_pawns = 0;
    //   int num_black_team_isolated_pawns = 0;
    //   for (chess::Square square : white_pawns){
    //     int file = chess::square_file(square);
    //     if (white_team_pawn_files.find(file) == white_team_pawn_files.end()){
    //       white_team_pawn_files.insert(file);
    //     }
    //   }
    //   for (chess::Square square : black_pawns){
    //     int file = chess::square_file(square);
    //     if (black_team_pawn_files.find(file) == black_team_pawn_files.end()){
    //       black_team_pawn_files.insert(file);
    //     }
    //   }
    //   for(chess::Square square : white_pawns){
    //     int file = chess::square_file(square);
    //     if (file > 0 || file < 7){
    //       if (white_team_pawn_files.find(file -1) == white_team_pawn_files.end() && white_team_pawn_files.find(file + 1) == white_team_pawn_files.end()){
    //         num_white_team_isolated_pawns++;
    //       }
    //     }
    //     else if (file == 0){
    //       if (white_team_pawn_files.find(file+1) == white_team_pawn_files.end()){
    //         num_white_team_isolated_pawns++;
    //       }
    //     }
    //     else if (file == 7){
    //       if (white_team_pawn_files.find(file-1) == white_team_pawn_files.end()){
    //         num_white_team_isolated_pawns++;
    //       }
    //     }
    //   }
    //   for(chess::Square square : black_pawns){
    //     int file = chess::square_file(square);
    //     if (file > 0 || file < 7){
    //       if (black_team_pawn_files.find(file -1) == black_team_pawn_files.end() && black_team_pawn_files.find(file + 1) == black_team_pawn_files.end()){
    //         num_black_team_isolated_pawns++;
    //       }
    //     }
    //     else if (file == 0){
    //       if (black_team_pawn_files.find(file+1) == black_team_pawn_files.end()){
    //         num_black_team_isolated_pawns++;
    //       }
    //     }
    //     else if (file == 7){
    //       if (black_team_pawn_files.find(file-1) == black_team_pawn_files.end()){
    //         num_black_team_isolated_pawns++;
    //       }
    //     }
    //   }
    //   return -num_white_team_isolated_pawns + num_black_team_isolated_pawns;
    // }

    int material_balance(const chess::Board &chessboard){
      int val = 0;
      std::unordered_map<chess::PieceType, int>  piece_dictionary = {{chess::PieceType::PAWN, 100}, {chess::PieceType::KNIGHT, 320}, {chess::PieceType::BISHOP, 325}, {chess::PieceType::ROOK, 500}, {chess::PieceType::QUEEN, 900}, {chess::PieceType::KING, 0}, {chess::PieceType::NONE, 0}};
      for(int sq = 0; sq < chess::constants::MAX_SQ; ++sq){
        chess::Piece piece = chessboard.at(chess::Square(sq));
        chess::PieceType piece_type = chess::utils::typeOfPiece(piece);
        if (chessboard.color(piece) == chess::Color::WHITE){
          val += piece_dictionary[piece_type];
        }
        else if (chessboard.color(piece) == chess::Color::BLACK){
          val -= piece_dictionary[piece_type];
        }
      }
      return val;
    }

    int mg_positional_evaluation(const chess::Board &chessboard){
      int val = 0;
      std::unordered_map<chess::PieceType, int> pieces_dict = {{chess::PieceType::PAWN, 0}, {chess::PieceType::KNIGHT, 1}, {chess::PieceType::BISHOP, 2}, {chess::PieceType::ROOK, 3}, {chess::PieceType::QUEEN, 4}, {chess::PieceType::KING, 5}};
      for(int sq = 0; sq < chess::constants::MAX_SQ; ++sq){
        chess::Piece piece = chessboard.at(chess::Square(sq));
        chess::PieceType piece_type = chess::utils::typeOfPiece(piece);
        if (piece_type != chess::PieceType::NONE){
          int pc = pieces_dict[piece_type];
          if (chessboard.color(piece) == chess::Color::BLACK){
            val -= mg_pesto_table[pc][sq];
          }
          else if (chessboard.color(piece) == chess::Color::WHITE){
            val += mg_pesto_table[pc][sq^56];
          }
        }
      }
      return val;
    }

    int eg_positional_evaluation(const chess::Board &chessboard){
      int val = 0;
      std::unordered_map<chess::PieceType, int> pieces_dict = {{chess::PieceType::PAWN, 0}, {chess::PieceType::KNIGHT, 1}, {chess::PieceType::BISHOP, 2}, {chess::PieceType::ROOK, 3}, {chess::PieceType::QUEEN, 4}, {chess::PieceType::KING, 5}};
      for(int sq = 0; sq < chess::constants::MAX_SQ; ++sq){
        chess::Piece piece = chessboard.at(chess::Square(sq));
        chess::PieceType piece_type = chess::utils::typeOfPiece(piece);
        if (piece_type != chess::PieceType::NONE){
          int pc = pieces_dict[piece_type];
          if (chessboard.color(piece) == chess::Color::BLACK){
            val -= eg_pesto_table[pc][sq];
          }
          else if (chessboard.color(piece) == chess::Color::WHITE){
            val += eg_pesto_table[pc][sq^56];
          }
        }
      }
      return val;
    }
    
    int minor_pieces_evaluation_function(const chess::Board &chessboard){
      int white_penalty = 0;
      int black_penalty = 0;  
      for(int sq = 0; sq < chess::constants::MAX_SQ; ++sq){
        chess::Piece piece = chessboard.at(chess::Square(sq));
        chess::PieceType piece_type = chess::utils::typeOfPiece(piece);
        if (piece_type == chess::PieceType::KNIGHT){
          int rank = sq >> 3;
          if (chessboard.color(piece) == chess::Color::WHITE && rank == 0){
            white_penalty--;
          }
          else if (chessboard.color(piece) == chess::Color::BLACK && rank == 7){
            black_penalty++;
          }
        }
        else if (piece_type == chess::PieceType::BISHOP){
          int rank = sq >> 3;
          if (chessboard.color(piece) == chess::Color::WHITE && rank == 0){
            white_penalty--;
          }
          else if (chessboard.color(piece) == chess::Color::BLACK && rank == 7){
            black_penalty++;
          }
        }
      }
      return white_penalty + black_penalty;
    }

    int mobility_evaluation(const chess::Board &chessboard){
      int num_white_team_moves = 0;
      int num_black_team_moves = 0;
      chess::Board tmp = chessboard;
      chess::Movelist legal_moves_1;
      chess::movegen::legalmoves(legal_moves_1, chessboard);
      tmp.makeNullMove();
      chess::Movelist legal_moves_2;
      chess::movegen::legalmoves(legal_moves_2, chessboard);
      tmp.unmakeNullMove();
      if (chessboard.sideToMove() == chess::Color::WHITE){
        num_white_team_moves = legal_moves_1.size();
        num_black_team_moves = legal_moves_2.size();
      }
      else if (chessboard.sideToMove() == chess::Color::BLACK){
        num_white_team_moves = legal_moves_2.size();
        num_black_team_moves = legal_moves_1.size();
      }
      return num_white_team_moves + -num_black_team_moves;
    }

    int king_pawn_evaluation(const chess::Board &chessboard){
      int num_white_king_pawns = 0;
      int num_black_king_pawns = 0;
      int white_king = chessboard.kingSq(chess::Color::WHITE);
      int black_king = chessboard.kingSq(chess::Color::BLACK);

      int white_king_file = white_king & 7;
      int black_king_file = black_king & 7;
      int white_king_rank = white_king >> 3;
      int black_king_rank = black_king >> 3;

      int white_king_left_file = white_king_file - 1;
      int black_king_left_file = black_king_file - 1;

      int white_king_right_file = white_king_file + 1;
      int black_king_right_file = black_king_file + 1;

      if (white_king_rank < 7){
        if (white_king_left_file != 3 && white_king_left_file != 4 && white_king_file > 0){
          chess::Square left_front_square = (chess::utils::makeSquare(chess::Rank(white_king_rank + 1), chess::File(white_king_left_file)));
          chess::Piece left_front_piece = chessboard.at(left_front_square);
          chess::PieceType left_front_piece_type = chess::utils::typeOfPiece(left_front_piece);
          if (chessboard.color(left_front_piece) == chess::Color::WHITE && left_front_piece_type == chess::PieceType::PAWN){
            num_white_king_pawns++;
          }
        }
        if (white_king_file != 3 && white_king_file != 4){
          chess::Square front_square = (chess::utils::makeSquare(chess::Rank(white_king_rank + 1), chess::File(white_king_file)));
          chess::Piece front_piece = chessboard.at(front_square);
          chess::PieceType front_piece_type = chess::utils::typeOfPiece(front_piece);
          if (chessboard.color(front_piece) == chess::Color::WHITE && front_piece_type == chess::PieceType::PAWN){
            num_white_king_pawns++;
          }
        }
        if (white_king_right_file != 3 && white_king_right_file != 4 && white_king_file < 7){
          chess::Square right_front_square = (chess::utils::makeSquare(chess::Rank(white_king_rank + 1), chess::File(white_king_right_file)));
          chess::Piece right_front_piece = chessboard.at(right_front_square);
          chess::PieceType right_front_piece_type = chess::utils::typeOfPiece(right_front_piece);
          if (chessboard.color(right_front_piece) == chess::Color::WHITE && right_front_piece_type == chess::PieceType::PAWN){
            num_white_king_pawns++;
          }
        }
      }
      if (black_king_rank > 0){
        if (black_king_left_file != 3 && black_king_left_file != 4 && black_king_file > 0){
          chess::Square left_front_square = (chess::utils::makeSquare(chess::Rank(black_king_rank - 1), chess::File(black_king_left_file)));
          chess::Piece left_front_piece = chessboard.at(left_front_square);
          chess::PieceType left_front_piece_type = chess::utils::typeOfPiece(left_front_piece);
          if (chessboard.color(left_front_piece) == chess::Color::BLACK && left_front_piece_type == chess::PieceType::PAWN){
            num_black_king_pawns++;
          }
        }
        if (black_king_file != 3 && black_king_file != 4){
          chess::Square front_square = (chess::utils::makeSquare(chess::Rank(black_king_rank - 1), chess::File(black_king_file)));
          chess::Piece front_piece = chessboard.at(front_square);
          chess::PieceType front_piece_type = chess::utils::typeOfPiece(front_piece);
          if (chessboard.color(front_piece) == chess::Color::BLACK && front_piece_type == chess::PieceType::PAWN){
            num_black_king_pawns++;
          }
        }
        if (black_king_right_file != 3 && black_king_right_file != 4 && black_king_file < 7){
          chess::Square right_front_square = (chess::utils::makeSquare(chess::Rank(black_king_rank - 1), chess::File(black_king_right_file)));
          chess::Piece right_front_piece = chessboard.at(right_front_square);
          chess::PieceType right_front_piece_type = chess::utils::typeOfPiece(right_front_piece);
          if (chessboard.color(right_front_piece) == chess::Color::BLACK && right_front_piece_type == chess::PieceType::PAWN){
            num_black_king_pawns++;
          }
        }
      }
      return num_white_king_pawns - num_black_king_pawns;
    }

    int center_pawn_evaluation_function(const chess::Board &chessboard){
      int king_pawn_square = 12;
      int queen_pawn_square = 11;
      int white_penalty = 0;
      int black_penalty = 0;
      chess::Piece white_piece_1 = chessboard.at(chess::Square(king_pawn_square));
      chess::PieceType white_piece_1_type = chess::utils::typeOfPiece(white_piece_1);
      if (white_piece_1_type == chess::PieceType::PAWN && chessboard.color(white_piece_1) == chess::Color::WHITE){
        white_penalty--;
      }
      chess::Piece white_piece_2 = chessboard.at(chess::Square(queen_pawn_square));
      chess::PieceType white_piece_2_type = chess::utils::typeOfPiece(white_piece_2);
      if (white_piece_2_type == chess::PieceType::PAWN && chessboard.color(white_piece_2) == chess::Color::WHITE){
        white_penalty--;
      }
      chess::Piece black_piece_1 = chessboard.at(chess::Square(king_pawn_square^56));
      chess::PieceType black_piece_1_type = chess::utils::typeOfPiece(black_piece_1);
      if (black_piece_1_type == chess::PieceType::PAWN && chessboard.color(black_piece_1) == chess::Color::BLACK){
        black_penalty++;
      }
      chess::Piece black_piece_2 = chessboard.at(chess::Square(queen_pawn_square^56));
      chess::PieceType black_piece_2_type = chess::utils::typeOfPiece(black_piece_2);
      if (black_piece_2_type == chess::PieceType::PAWN && chessboard.color(black_piece_2) == chess::Color::BLACK){
        black_penalty++;
      }
      return white_penalty + black_penalty;
    }

    int king_attack_evaluation(const chess::Board &chessboard){
      chess::Square white_king = chessboard.kingSq(chess::Color::WHITE);
      chess::Square black_king = chessboard.kingSq(chess::Color::BLACK);
      int white_distance_val = 0;
      int black_distance_val = 0;
      for (int sq = 0; sq < chess::constants::MAX_SQ; sq++){
        chess::Piece piece = chessboard.at(chess::Square(sq));
        chess::PieceType piece_type = chess::utils::typeOfPiece(piece);
        if (piece_type != chess::PieceType::NONE){
          if (chessboard.color(piece) == chess::Color::BLACK){
            black_distance_val += chess::utils::squareDistance(white_king, chess::Square(sq));
          }
          else if (chessboard.color(piece) == chess::Color::WHITE){
            white_distance_val -= chess::utils::squareDistance(black_king, chess::Square(sq));
          }
        }
      }
      return white_distance_val + black_distance_val;
    }
    
    int mg_evaluation_function(const chess::Board &chessboard){
      int val = crowded_pawns_evaluation(chessboard) * 10 + passed_pawns_evaluation(chessboard) + material_balance(chessboard) + mg_positional_evaluation(chessboard) + mobility_evaluation(chessboard) * 3 + king_pawn_evaluation(chessboard) * 5 + minor_pieces_evaluation_function(chessboard)*20 + king_attack_evaluation(chessboard) + center_pawn_evaluation_function(chessboard) * 5;
      return val;
    }

    float eg_evaluation_function(const chess::Board &chessboard){
      int val = crowded_pawns_evaluation(chessboard) * 5 + passed_pawns_evaluation(chessboard)*5 + material_balance(chessboard) + eg_positional_evaluation(chessboard) + mobility_evaluation(chessboard) * 5 + king_pawn_evaluation(chessboard) * 5 + minor_pieces_evaluation_function(chessboard)*20 + king_attack_evaluation(chessboard) * 5 + center_pawn_evaluation_function(chessboard) * 5;
      return val;
    }

    float nnue_evaluation_function(const chess::Board &chessboard, const chess::Movelist &moves){
      bool checkmate = (moves.empty() && chessboard.inCheck());
      bool stalemate = (moves.empty() && !chessboard.inCheck());
      if (checkmate){
        return -CHECKMATE_VAL;
      }
      if (stalemate || chessboard.isInsufficientMaterial() || chessboard.isRepetition(1) || chessboard.isHalfMoveDraw()){
        return 0;
      }
      float eval = nnue_evaluate_fen(chessboard.getFen().c_str());
      return eval;
    }

    float evaluation_function(const chess::Board &chessboard, const chess::Movelist &moves){
      bool checkmate = (moves.empty() && chessboard.inCheck());
      bool stalemate = (moves.empty() && !chessboard.inCheck());
      if (checkmate){
        return -CHECKMATE_VAL;
      }
      if (stalemate || chessboard.isInsufficientMaterial() || chessboard.isRepetition(1)  || chessboard.isHalfMoveDraw()){
        return 0;
      }
      std::unordered_map<chess::PieceType, int> pieces_dict = {{chess::PieceType::PAWN, 0}, {chess::PieceType::KNIGHT, 1}, {chess::PieceType::BISHOP, 2}, {chess::PieceType::ROOK, 3}, {chess::PieceType::QUEEN, 4}, {chess::PieceType::KING, 5}};
      int mg_phase = 0;
      for(int sq = 0; sq < chess::constants::MAX_SQ; ++sq){
        chess::Piece piece = chessboard.at(chess::Square(sq));
        chess::PieceType piece_type = chess::utils::typeOfPiece(piece);
        if (piece_type != chess::PieceType::NONE){
          int pc = pieces_dict[piece_type];
          mg_phase += gamephaseInc[pc];
        }
      }
      int mg_eval = mg_evaluation_function(chessboard);
      int eg_eval = eg_evaluation_function(chessboard);
      if (mg_phase > 24){
        mg_phase = 24;
      }
      int eg_phase = 24 - mg_phase;
      float val = (mg_eval * mg_phase + eg_eval * eg_phase) / 24.0;
      if (chessboard.sideToMove() == chess::Color::WHITE){
        return val;
      }
      return -val;
    }


};

#endif