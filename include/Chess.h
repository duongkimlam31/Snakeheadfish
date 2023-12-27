#ifndef GAME_H
#define GAME_H
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cctype>
#include <string>
#include <vector>

#include "Snakeheadfish.h"
#include "Chessboard.h"

class Chess {
 private:
  Chessboard *chessboard;
  Snakeheadfish *snakeheadfish;
  chess::Board *ai_chessboard;

 public:
  Chess(Chessboard *chessboard) {
    char book_path[] = "../book_openings/gm2600.bin";
    this->chessboard = chessboard;
    std::cout << "Setting up...\n";
    this->ai_chessboard = new chess::Board();
    this->snakeheadfish = new Snakeheadfish(book_path, 10);
    this->chessboard->setup();
    if (system("clear") == -1) {
      exit(-1);
    }
  }

  ~Chess() {
    delete this->chessboard;
    delete this->snakeheadfish;
    delete this->ai_chessboard;
  };

  bool inCheck(int turn, Chessboard *board, Chesspiece *king) {
    bool check = false;
    if (turn % 2 == 0) {
      std::set<std::string> *black_team_moves =
          board->getBlackTeamAvailableMoves();
      auto it = black_team_moves->find(king->getPosition());
      check = (it != black_team_moves->end());
    } else {
      std::set<std::string> *white_team_moves =
          board->getWhiteTeamAvailableMoves();
      auto it = white_team_moves->find(king->getPosition());
      check = (it != white_team_moves->end());
    }
    return check;
  }

  bool inCheckForSimulation(int turn, Chessboard *board) {
    bool check = false;
    std::string current_team = "";
    std::set<std::string> enemy_team_moves;
    if (turn % 2 == 0) {
      current_team = "white";
      enemy_team_moves = *board->getBlackTeamAvailableMoves();
    } else {
      current_team = "black";
      enemy_team_moves = *board->getWhiteTeamAvailableMoves();
    }
    Chesspiece *piece;
    bool stop_loop = false;
    for (int i = 0; i < 8; ++i) {
      if (stop_loop) {
        break;
      }
      for (int j = 0; j < 8; ++j) {
        piece = board->getBoard()->at(i).at(j)->getPiece();
        if (piece != nullptr && piece->getTeam() == current_team &&
            piece->getName().find("King") != std::string::npos) {
          stop_loop = true;
          break;
        }
      }
    }
    auto it = enemy_team_moves.find(piece->getPosition());
    check = (it != enemy_team_moves.end());
    return check;
  }

  void pruneAvailableMoves(std::set<std::string> legalMoves, int turn) {
    Team *team;
    if (turn % 2 == 0) {
      team = this->chessboard->getWhiteTeam();
    } else {
      team = this->chessboard->getBlackTeam();
    }
    std::vector<Chesspiece *> team_pieces = team->getPieces();
    for (int i = 0; i < team_pieces.size(); ++i) {
      if (team_pieces.at(i)->getStatus() != "captured") {
        std::vector<std::string> moves = team_pieces.at(i)->getAvailableMoves();
        std::vector<std::string> prunedMoves;
        Chessboard next_state;
        for (int j = 0; j < moves.size(); ++j) {
          next_state = *this->chessboard;
          Cell c1 = *next_state.getCell(team_pieces.at(i)->getPosition());
          Cell c2 = *next_state.getCell(moves.at(j));
          next_state.movePiece(&c1, &c2);
          if (turn % 2 == 0) {
            next_state.generateBlackTeamMoves();
          } else {
            next_state.generateWhiteTeamMoves();
          }
          if (!inCheckForSimulation(turn, &next_state)) {
            prunedMoves.push_back(c2.getName());
          }
        }
        team_pieces.at(i)->setAvailableMoves(prunedMoves);
      }
    }
  }

  void addPoints(int points, int turn) {
    Team *current_team;
    if (turn % 2 == 0) {
      current_team = this->chessboard->getWhiteTeam();
    } else {
      current_team = this->chessboard->getBlackTeam();
    }
    current_team->addPoints(points);
  }

  std::vector<std::string> legalMoveCheck(std::string starting_location,
                                          Chessboard *board, int turn) {
    std::vector<std::string> legalMoves;
    bool legal = false;
    Cell c1 = *board->getCell(starting_location);
    Chessboard next_state;
    std::vector<std::string> moves = c1.getPiece()->getAvailableMoves();
    for (int i = 0; i < moves.size(); ++i) {
      next_state = *board;
      c1 = *next_state.getCell(starting_location);
      Cell c2 = *next_state.getCell(moves.at(i));
      next_state.movePiece(&c1, &c2);
      if (turn % 2 == 0) {
        next_state.generateBlackTeamMoves();
      } else {
        next_state.generateWhiteTeamMoves();
      }
      if (!inCheckForSimulation(turn, &next_state)) {
        legalMoves.push_back(c2.getName());
      }
    }
    return legalMoves;
  }

  std::set<std::string> allLegalMoves(int turn) {
    std::set<std::string> all_legal_moves;
    std::vector<Chesspiece *> current_team;
    if (turn % 2 == 0) {
      current_team = this->chessboard->getWhiteTeam()->getPieces();
    } else {
      current_team = this->chessboard->getBlackTeam()->getPieces();
    }
    for (int i = 0; i < current_team.size(); ++i) {
      std::string position = current_team.at(i)->getPosition();
      if (current_team.at(i)->getStatus() != "captured") {
        std::vector<std::string> tmp_legal_moves =
            legalMoveCheck(position, this->chessboard, turn);
        if (tmp_legal_moves.size() > 0) {
          all_legal_moves.insert(current_team.at(i)->getPosition());
          for (int j = 0; j < tmp_legal_moves.size(); ++j) {
            all_legal_moves.insert(tmp_legal_moves.at(j));
          }
        }
      }
    }
    return all_legal_moves;
  }

  void start(std::string player_team) {
    std::string move_sequence = "";
    std::string input;
    std::string input_move;
    int turn = 0;
    bool announced = false;
    bool in_check = false;
    bool fifty_fifty_draw = false;
    bool player_turn;
    bool nextTurn;
    std::string ai_move = "";
    Cell *c1 = NULL;
    Cell *c2 = NULL;
    int promotion_val = 0;
    if (player_team == "white") {
      player_turn = true;
    } else {
      player_turn = false;
    }
    while (true) {
      chess::Movelist ai_board_legal_moves;
      chess::movegen::legalmoves(ai_board_legal_moves, *ai_chessboard);
      bool checkmate = (ai_board_legal_moves.empty() && ai_chessboard->inCheck());
      bool stalemate = (ai_board_legal_moves.empty() && !ai_chessboard->inCheck());
      if (checkmate || stalemate || ai_chessboard->isInsufficientMaterial() || ai_chessboard->isRepetition(2) || ai_chessboard->isHalfMoveDraw()){
        break;
      }
      if (!announced) {
        this->chessboard->generateAllMoves();
        if (turn % 2 == 0) {
          if (inCheck(turn, this->chessboard,
                      this->chessboard->getWhiteTeam()->getKing())) {
            in_check = true;
            this->chessboard->getWhiteTeam()->getKing()->setStatus("checked");
            this->chessboard->getWhiteTeam()->getKing()->changeColor();
            std::cout << "White team is in check.\n" << std::endl;
          }
        } else if (turn % 2 != 0) {
          if (inCheck(turn, this->chessboard,
                      this->chessboard->getBlackTeam()->getKing())) {
            in_check = true;
            this->chessboard->getBlackTeam()->getKing()->setStatus("checked");
            this->chessboard->getBlackTeam()->getKing()->changeColor();
            std::cout << "Black team is in check.\n" << std::endl;
          }
        }
        if (in_check) {
          this->chessboard->removeCastleStatus();
          this->chessboard->generateAllMoves();
        }
        std::set<std::string> all_legal_moves;
        all_legal_moves = allLegalMoves(turn);
        pruneAvailableMoves(all_legal_moves, turn);
        if (in_check && player_turn) {
          this->chessboard->showLegalMoves(all_legal_moves, turn);
        }
        if (system("clear") == -1) {
          exit(-1);
        }
        if (player_team == "white") {
          this->chessboard->printBoard();
        } else {
          this->chessboard->printBoardReverse();
        }
  
        std::cout.flush();
        if (player_turn){
          std::cout << "Snakehead fish's move: " << ai_move << std::endl;
        }
      }
      if (player_turn) {
        if (turn % 2 != 0 && !announced) {
          std::cout << "Enter 0 to exit.\n";
          std::cout << "Black team's turn: \n";
          announced = true;
        } else if (turn % 2 == 0 && !announced) {
          std::cout << "Enter 0 to exit.\n";
          std::cout << "White team's turn: \n";
          announced = true;
        }
        std::cout << "Choose a square: ";
        getline(std::cin, input);
        if (input == "0") {
          break;
        }
        if (input.size() != 2) {
          std::cout << "Invalid Input. Please try again. \n";
          continue;
        }
        char col_name = input.at(0);
        if (!isalpha(col_name)) {
          std::cout << "Invalid Input. Please try again. \n";
          continue;
        }
        col_name = toupper(col_name);
        if (col_name < 'A' or col_name > 'H') {
          std::cout << "Invalid Input. Please try again. \n";
          continue;
        }
        char row_name = input.at(1);
        if (!isdigit(row_name) or row_name < '1' or row_name > '8') {
          std::cout << "Invalid Input. Please try again. \n";
          continue;
        }
        c1 = this->chessboard->getCell(input);
        if (this->chessboard->getCell(input)->getPiece() == nullptr) {
          std::cout << "This cell is empty. Please try a different cell. \n";
          continue;
        }
        if (turn % 2 == 0 && c1->getPiece()->getTeam() != "white") {
          std::cout
              << "This piece does not belong to your team. Please choose a "
                 "different one. \n";
          continue;
        }
        if (turn % 2 != 0 && c1->getPiece()->getTeam() != "black") {
          std::cout
              << "This piece does not belong to your team. Please choose a "
                 "different one. \n";
          continue;
        }
        this->chessboard->removeAvailableMoves();
        this->chessboard->showAvailableMoves(c1->getName());
        if (system("clear") == -1) {
          exit(-1);
        }
        if (player_team == "white") {
          this->chessboard->printBoard();
        } else {
          this->chessboard->printBoardReverse();
        }
  
        std::cout.flush();
        nextTurn = false;
        bool announced_2 = false;
        while (true) {
          if (!announced_2) {
            std::cout << "Press 1 to go back." << std::endl;
            announced_2 = true;
          }
          std::cout << "Choose a square: ";
          getline(std::cin, input_move);
          if (input_move == "1") {
            this->chessboard->removeAvailableMoves();
            announced = false;
            break;
          }
          bool canMove = false;
          if (input_move.size() != 2) {
            std::cout << "Invalid Input. Please try again. \n";
            continue;
          }
          char col_name_move = input_move.at(0);
          if (!isalpha(col_name_move)) {
            std::cout << "Invalid Input. Please try again. \n";
            continue;
          }
          col_name_move = toupper(col_name_move);
          if (col_name_move < 'A' or col_name_move > 'H') {
            std::cout << "Invalid Input. Please try again. \n";
            continue;
          }
          char row_name_move = input_move.at(1);
          if (!isdigit(row_name_move) or row_name_move < '1' or
              row_name_move > '8') {
            std::cout << "Invalid Input. Please try again. \n";
            continue;
          }
          std::vector<std::string> legalMoveChecks =
              c1->getPiece()->getAvailableMoves();
          for (int i = 0; i < legalMoveChecks.size(); ++i) {
            if (toupper(legalMoveChecks.at(i).at(0)) ==
                    toupper(input_move.at(0)) &&
                legalMoveChecks.at(i).at(1) == input_move.at(1)) {
              canMove = true;
              break;
            }
          }
          if (canMove) {
            nextTurn = true;
            c2 = this->chessboard->getCell(input_move);
            player_turn = false;
            break;
          } else {
            std::cout << "That move is not legal. Please try again. \n";
          }
        }
      } else {
        std::cout << "Thinking...\n\n";
        this->snakeheadfish->lazy_smp(*ai_chessboard);
        ai_move = chess::uci::moveToSan(*ai_chessboard, this->snakeheadfish->get_next_move());
        ai_chessboard->makeMove(this->snakeheadfish->get_next_move());
        std::string move = chess::uci::moveToUci(snakeheadfish->get_next_move());
        if (move.size() == 5) {
          if (move.at(move.size() - 1) == 'q') {
            promotion_val = 1;
          } else if (move.at(move.size() - 1) == 'r') {
            promotion_val = 2;
          } else if (move.at(move.size() - 1) == 'b') {
            promotion_val = 3;
          } else if (move.at(move.size() - 1) == 'n') {
            promotion_val = 4;
          }
        }
        std::string starting_location = "";
        std::string destination = "";
        starting_location += move[0];
        starting_location += move[1];
        destination += move[2];
        destination += move[3];
        c1 = this->chessboard->getCell(starting_location);
        c2 = this->chessboard->getCell(destination);
        player_turn = true;
        nextTurn = true;
      }
      if (nextTurn) {
        int points = this->chessboard->movePiece(c1, c2);
        move_sequence += tolower(c1->getName().at(0));
        move_sequence += c1->getName().at(1);
        move_sequence += tolower(c2->getName().at(0));
        move_sequence += c2->getName().at(1);
        move_sequence += " ";
        addPoints(points, turn);
        int player_promotion_val = 0;
        if (this->chessboard->getPromotionAvailable()) {
          if (turn % 2 == 0) {
            player_promotion_val = this->chessboard->promote(
                this->chessboard->getWhiteTeam(), c2, promotion_val);
          } else {
            player_promotion_val = this->chessboard->promote(
                this->chessboard->getBlackTeam(), c2, promotion_val);
          }
          this->chessboard->setPromotionAvailable(false);
        }
        if (!player_turn) {
          std::string move = "";
          move += tolower(input.at(0));
          move += input.at(1);
          move += tolower(input_move.at(0));
          move += input_move.at(1);
          if (player_promotion_val == 1) {
            move += 'q';
            chess::Move ai_board_move = chess::uci::uciToMove(*ai_chessboard, move);
            this->ai_chessboard->makeMove(ai_board_move);
          }
          else if (player_promotion_val == 2) {
            move += 'r';
            chess::Move ai_board_move = chess::uci::uciToMove(*ai_chessboard, move);
            this->ai_chessboard->makeMove(ai_board_move);
          }
          else if (player_promotion_val == 3) {
            move += 'b';
            chess::Move ai_board_move = chess::uci::uciToMove(*ai_chessboard, move);
            this->ai_chessboard->makeMove(ai_board_move);
          }
          else if (player_promotion_val == 4) {
            move += 'n';
            chess::Move ai_board_move = chess::uci::uciToMove(*ai_chessboard, move);
            this->ai_chessboard->makeMove(ai_board_move);
          }
          else{
            chess::Move ai_board_move = chess::uci::uciToMove(*ai_chessboard, move);
            this->ai_chessboard->makeMove(ai_board_move);
          }
        }
        announced = false;
        ++turn;
        in_check = false;
        promotion_val = 0;
        this->chessboard->removeAvailableMoves();
        if (this->chessboard->getWhiteTeam()->getKing()->getStatus() ==
            "checked") {
          this->chessboard->getWhiteTeam()->getKing()->setStatus("active");
          this->chessboard->getWhiteTeam()->getKing()->changeColor();
        } else if (this->chessboard->getBlackTeam()->getKing()->getStatus() ==
                   "checked") {
          this->chessboard->getBlackTeam()->getKing()->setStatus("active");
          this->chessboard->getBlackTeam()->getKing()->changeColor();
        }
        this->chessboard->removeEnPassant(turn);
        this->chessboard->removeCastleStatus();
      }
    }
    this->chessboard->getWhiteTeam()->getKing()->setStatus("captured");
    this->chessboard->getBlackTeam()->getKing()->setStatus("captured");
    this->chessboard->getWhiteTeam()->getKing()->changeColor();
    this->chessboard->getBlackTeam()->getKing()->changeColor();
    if (system("clear") == -1) {
      exit(-1);
    }
    std::cout << "Move sequence: " << move_sequence << std::endl;
    if (player_team == "white") {
      this->chessboard->printBoard();
    } else {
      this->chessboard->printBoardReverse();
    }
    std::cout.flush();
    chess::Movelist ai_board_legal_moves;
    chess::movegen::legalmoves(ai_board_legal_moves, *ai_chessboard);
    bool checkmate = (ai_board_legal_moves.empty() && ai_chessboard->inCheck());
    bool stalemate = (ai_board_legal_moves.empty() && !ai_chessboard->inCheck());
    if (ai_chessboard->isRepetition(2)) {
      std::cout << "Draw by repetition" << std::endl;
    } 
    if (ai_chessboard->isHalfMoveDraw()) {
      std::cout << "Draw by fifty-move rule" << std::endl;
    } 
    else if (ai_chessboard->isInsufficientMaterial()) {
      std::cout << "Draw by insufficient material" << std::endl;
    } else if (stalemate) {
      std::cout << "Stalemate" << std::endl;
    } else if (turn % 2 != 0 && checkmate) {
      std::cout << "Checkmate" << std::endl;
      std::cout << "The white team has won" << std::endl;
    } else if (turn % 2 == 0 && checkmate) {
      std::cout << "Checkmate" << std::endl;
      std::cout << "The black team has won" << std::endl;
    }
    std::cout << "White team's total points: "
              << this->chessboard->getWhiteTeam()->getPoints() << std::endl;
    std::cout << "Black team's total points: "
              << this->chessboard->getBlackTeam()->getPoints() << std::endl;
  }
};

#endif