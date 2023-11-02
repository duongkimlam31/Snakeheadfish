#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "Cell.h"
#include "Chesspiece.h"
#include "Team.h"
#include "cstring"

class Chessboard {
 private:
  Team *white;
  Team *black;
  std::vector<std::vector<Cell *>> *board =
      new std::vector<std::vector<Cell *>>;
  std::unordered_map<std::string, std::string> *occupiedCells =
      new std::unordered_map<std::string, std::string>;
  std::set<std::string> *whiteTeamAvailableMoves = new std::set<std::string>;
  std::set<std::string> *blackTeamAvailableMoves = new std::set<std::string>;
  bool promotionAvailable;
  std::string enPassantAvailable;
  std::string enPassantPawn;
  int full_move;
  int half_move;
  char fen_en_passant_location[3];

 public:
  Chessboard() {
    for (int i = 0; i < 8; ++i) {
      std::string row = std::to_string(8 - i);
      board->push_back({});
      for (int j = 0; j < 8; ++j) {
        char col = char(65 + j);
        std::string cell_name = col + row;
        Cell *c = new Cell(cell_name);
        board->at(i).push_back(c);
      }
    }
    this->promotionAvailable = false;
    this->enPassantAvailable = "";
    this->enPassantPawn = "";
    this->white = new Team("white");
    this->black = new Team("black");
    this->half_move = 0;
    this->full_move = 1;
    strcpy(this->fen_en_passant_location, "-");
  }
  ~Chessboard() {
    delete white;
    delete black;
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        delete board->at(i).at(j);
      }
    }
    delete board;
    delete occupiedCells;
    delete whiteTeamAvailableMoves;
    delete blackTeamAvailableMoves;
  }

  Chessboard &operator=(const Chessboard &origObject) {
    if (this != &origObject) {
      // Delete the existing resources
      delete this->white;
      delete this->black;
      for (std::vector<Cell *> &row : *board) {
        for (Cell *cell : row) {
          delete cell;
        }
      }
      delete this->board;
      delete this->occupiedCells;
      delete this->whiteTeamAvailableMoves;
      delete this->blackTeamAvailableMoves;

      // Create new resources and deep copy the contents
      this->board = new std::vector<std::vector<Cell *>>;
      for (int i = 0; i < 8; ++i) {
        std::string row = std::to_string(8 - i);
        this->board->push_back({});
        for (int j = 0; j < 8; ++j) {
          char col = char(65 + j);
          std::string cell_name = col + row;
          Cell *c = new Cell(cell_name);
          this->board->at(i).push_back(c);
        }
      }

      occupiedCells = new std::unordered_map<std::string, std::string>(
          *origObject.occupiedCells);
      whiteTeamAvailableMoves =
          new std::set<std::string>(*origObject.whiteTeamAvailableMoves);
      blackTeamAvailableMoves =
          new std::set<std::string>(*origObject.blackTeamAvailableMoves);
      this->promotionAvailable = origObject.promotionAvailable;
      this->enPassantAvailable = origObject.enPassantAvailable;
      this->enPassantPawn = origObject.enPassantPawn;
      this->white = new Team(*(origObject.white));
      this->black = new Team(*(origObject.black));
      for (int i = 0; i < this->white->getPieces().size(); ++i) {
        if (this->white->getPieces().at(i)->getStatus() == "captured") {
          continue;
        }
        std::string tmp_position =
            this->white->getPieces().at(i)->getPosition();
        Cell *tmp_cell = getCell(tmp_position);
        tmp_cell->setPiece(this->white->getPieces().at(i));
      }
      for (int i = 0; i < this->black->getPieces().size(); ++i) {
        std::string tmp_position =
            this->black->getPieces().at(i)->getPosition();
        if (this->black->getPieces().at(i)->getStatus() == "captured") {
          continue;
        }
        Cell *tmp_cell = getCell(tmp_position);
        tmp_cell->setPiece(this->black->getPieces().at(i));
      }
    }
    return *this;
  }

  void setup() {
    Chesspiece *wp1 = new Pawn("White Pawn 1", "white", "A2");
    Chesspiece *wp2 = new Pawn("White Pawn 2", "white", "B2");
    Chesspiece *wp3 = new Pawn("White Pawn 3", "white", "C2");
    Chesspiece *wp4 = new Pawn("White Pawn 4", "white", "D2");
    Chesspiece *wp5 = new Pawn("White Pawn 5", "white", "E2");
    Chesspiece *wp6 = new Pawn("White Pawn 6", "white", "F2");
    Chesspiece *wp7 = new Pawn("White Pawn 7", "white", "G2");
    Chesspiece *wp8 = new Pawn("White Pawn 8", "white", "H2");

    Chesspiece *bp1 = new Pawn("Black Pawn 1", "black", "A7");
    Chesspiece *bp2 = new Pawn("Black Pawn 2", "black", "B7");
    Chesspiece *bp3 = new Pawn("Black Pawn 3", "black", "C7");
    Chesspiece *bp4 = new Pawn("Black Pawn 4", "black", "D7");
    Chesspiece *bp5 = new Pawn("Black Pawn 5", "black", "E7");
    Chesspiece *bp6 = new Pawn("Black Pawn 6", "black", "F7");
    Chesspiece *bp7 = new Pawn("Black Pawn 7", "black", "G7");
    Chesspiece *bp8 = new Pawn("Black Pawn 8", "black", "H7");

    Chesspiece *wr1 = new Rook("White Rook 1", "white", "A1");
    Chesspiece *wr2 = new Rook("White Rook 2", "white", "H1");
    Chesspiece *br1 = new Rook("Black Rook 1", "black", "A8");
    Chesspiece *br2 = new Rook("Black Rook 2", "black", "H8");

    Chesspiece *wk1 = new Knight("White Knight 1", "white", "B1");
    Chesspiece *wk2 = new Knight("White Knight 2", "white", "G1");
    Chesspiece *bk1 = new Knight("Black Knight 1", "black", "B8");
    Chesspiece *bk2 = new Knight("Black Knight 2", "black", "G8");

    Chesspiece *wb1 = new Bishop("White Bishop 1", "white", "C1");
    Chesspiece *wb2 = new Bishop("White Bishop 2", "white", "F1");
    Chesspiece *bb1 = new Bishop("Black Bishop 1", "black", "C8");
    Chesspiece *bb2 = new Bishop("Black Bishop 2", "black", "F8");

    Chesspiece *wk = new King("White King", "white", "E1");
    Chesspiece *bk = new King("Black King", "black", "E8");

    Chesspiece *wq = new Queen("White Queen", "white", "D1");
    Chesspiece *bq = new Queen("Black Queen", "black", "D8");

    std::vector<Chesspiece *> row1 = {wr1, wk1, wb1, wq, wk, wb2, wk2, wr2};
    std::vector<Chesspiece *> row2 = {wp1, wp2, wp3, wp4, wp5, wp6, wp7, wp8};
    std::vector<Chesspiece *> row7 = {bp1, bp2, bp3, bp4, bp5, bp6, bp7, bp8};
    std::vector<Chesspiece *> row8 = {br1, bk1, bb1, bq, bk, bb2, bk2, br2};
    white->setKing(dynamic_cast<King *>(wk));
    black->setKing(dynamic_cast<King *>(bk));

    for (int i = 0; i < 8; ++i) {
      this->black->addPiece(row7.at(i));
      this->black->addPiece(row8.at(i));
      this->board->at(0).at(i)->setPiece(row8.at(i));
      this->board->at(1).at(i)->setPiece(row7.at(i));
      this->white->addPiece(row2.at(i));
      this->white->addPiece(row1.at(i));
      this->board->at(6).at(i)->setPiece(row2.at(i));
      this->board->at(7).at(i)->setPiece(row1.at(i));
    }
    findOccupiedCells();
  }

  std::vector<std::vector<Cell *>> *getBoard() { return this->board; }
  std::set<std::string> *getWhiteTeamAvailableMoves() {
    return this->whiteTeamAvailableMoves;
  }
  std::set<std::string> *getBlackTeamAvailableMoves() {
    return this->blackTeamAvailableMoves;
  }
  std::string getEnPassantAvailability() { return this->enPassantAvailable; }
  bool getPromotionAvailable() { return this->promotionAvailable; }
  Team *getWhiteTeam() { return this->white; }
  Team *getBlackTeam() { return this->black; }
  void setPromotionAvailable(bool promotionAvailable) {
    this->promotionAvailable = promotionAvailable;
  }
  void setEnPassantAvailability(std::string enPassantAvailable) {
    this->enPassantAvailable = enPassantAvailable;
  }
  void setBoard(std::vector<std::vector<Cell *>> *board) {
    this->board = board;
  }

  void checkCastleStatus() {
    std::string order = "";
    generateWhiteTeamMoves();
    generateBlackTeamMoves();
    if (this->white->getKing()->getStatus() != "checked" &&
        !this->white->getKing()->getMoved()) {
      for (int i = 0; i < 5; ++i) {
        if (this->board->at(7).at(i)->getPiece() != NULL &&
            this->board->at(7).at(i)->getPiece()->getName().find("King") !=
                std::string::npos) {
          order += "k";
        } else if (this->board->at(7).at(i)->getPiece() != NULL &&
                   this->board->at(7).at(i)->getPiece()->getName().find(
                       "Rook") != std::string::npos) {
          order += "r";
        } else if (this->board->at(7).at(i)->getPiece() == NULL) {
          order += "n";
        }
      }
      if (order == "rnnnk") {
        Rook *r = dynamic_cast<Rook *>(this->board->at(7).at(0)->getPiece());
        auto it = blackTeamAvailableMoves->find("D1");
        if (!r->getMoved() && (it == blackTeamAvailableMoves->end())) {
          this->white->getKing()->setCastleLeft(true);
          r->setCastleLocation("D1");
        }
      }
      order = "";
      for (int i = 4; i < 8; ++i) {
        if (this->board->at(7).at(i)->getPiece() != NULL &&
            this->board->at(7).at(i)->getPiece()->getName().find("King") !=
                std::string::npos) {
          order += "k";
        } else if (this->board->at(7).at(i)->getPiece() != NULL &&
                   this->board->at(7).at(i)->getPiece()->getName().find(
                       "Rook") != std::string::npos) {
          order += "r";
        } else if (this->board->at(7).at(i)->getPiece() == NULL) {
          order += "n";
        }
      }
      if (order == "knnr") {
        Rook *r = dynamic_cast<Rook *>(this->board->at(7).at(7)->getPiece());
        auto it = blackTeamAvailableMoves->find("F1");
        if (!r->getMoved() && it == blackTeamAvailableMoves->end()) {
          this->white->getKing()->setCastleRight(true);
          r->setCastleLocation("F1");
        }
      }
    }
    if (this->black->getKing()->getStatus() != "checked" &&
        !this->black->getKing()->getMoved()) {
      order = "";
      for (int i = 0; i < 5; ++i) {
        if (this->board->at(0).at(i)->getPiece() != NULL &&
            this->board->at(0).at(i)->getPiece()->getName().find("King") !=
                std::string::npos) {
          order += "k";
        } else if (this->board->at(0).at(i)->getPiece() != NULL &&
                   this->board->at(0).at(i)->getPiece()->getName().find(
                       "Rook") != std::string::npos) {
          order += "r";
        } else if (this->board->at(0).at(i)->getPiece() == NULL) {
          order += "n";
        }
      }
      if (order == "rnnnk") {
        Rook *r = dynamic_cast<Rook *>(this->board->at(0).at(0)->getPiece());
        auto it = whiteTeamAvailableMoves->find("D8");
        if (!r->getMoved() && it == whiteTeamAvailableMoves->end()) {
          this->black->getKing()->setCastleLeft(true);
          r->setCastleLocation("D8");
        }
      }
      order = "";
      for (int i = 4; i < 8; ++i) {
        if (this->board->at(0).at(i)->getPiece() != NULL &&
            this->board->at(0).at(i)->getPiece()->getName().find("King") !=
                std::string::npos) {
          order += "k";
        } else if (this->board->at(0).at(i)->getPiece() != NULL &&
                   this->board->at(0).at(i)->getPiece()->getName().find(
                       "Rook") != std::string::npos) {
          order += "r";
        } else if (this->board->at(0).at(i)->getPiece() == NULL) {
          order += "n";
        }
      }
      if (order == "knnr") {
        Rook *r = dynamic_cast<Rook *>(this->board->at(0).at(7)->getPiece());
        auto it = whiteTeamAvailableMoves->find("F8");
        if (!r->getMoved() && it == whiteTeamAvailableMoves->end()) {
          this->black->getKing()->setCastleRight(true);
          r->setCastleLocation("F8");
        }
      }
    }
  }

  void removeEnPassant(int turn) {
    Team *team_to_remove;
    if (turn % 2 == 0 && getEnPassantAvailability() == "black") {
      team_to_remove = this->black;
    } else if (turn % 2 != 0 && getEnPassantAvailability() == "white") {
      team_to_remove = this->white;
    } else {
      return;
    }
    std::vector<Chesspiece *> pieces = team_to_remove->getPieces();
    for (int i = 0; i < pieces.size(); ++i) {
      if (pieces.at(i)->getName().find("Pawn") != std::string::npos) {
        Pawn *p = dynamic_cast<Pawn *>(pieces.at(i));
        p->setEnPassant("");
        enPassantAvailable = "";
        enPassantPawn = "";
      }
    }
  }

  void removeCastleStatus() {
    if (this->white->getKing()->getCastleLeft()) {
      this->white->getKing()->setCastleLeft(false);
    }
    if (this->white->getKing()->getCastleRight()) {
      this->white->getKing()->setCastleRight(false);
    }
    if (this->black->getKing()->getCastleLeft()) {
      this->black->getKing()->setCastleLeft(false);
    }
    if (this->black->getKing()->getCastleRight()) {
      this->black->getKing()->setCastleRight(false);
    }
    for (int i = 0; i < 16; ++i) {
      Rook *r;
      if (this->white->getPieces().at(i)->getName().find("Rook") !=
          std::string::npos) {
        r = dynamic_cast<Rook *>(this->white->getPieces().at(i));
        r->setCastleLocation("");
      }
      if (this->black->getPieces().at(i)->getName().find("Rook") !=
          std::string::npos) {
        r = dynamic_cast<Rook *>(this->black->getPieces().at(i));
        r->setCastleLocation("");
      }
    }
  }

  void printBoard() {
    std::string s = "    A   B   C   D   E   F   G   H\n";
    int row_num = 0;
    for (int i = 0; i < 16; ++i) {
      if (i % 2 == 0) {
        s += "  +---+---+---+---+---+---+---+---+\n";
      } else {
        std::string row_num_s = std::to_string(8 - row_num);
        std::string tmp = "";
        tmp += row_num_s + " ";
        for (int j = 0; j < 8; ++j) {
          tmp += "| ";
          if (this->board->at(row_num).at(j)->getPiece() == nullptr) {
            tmp += this->board->at(row_num).at(j)->getPrintStatus() + " ";
          } else {
            tmp += board->at(row_num).at(j)->getPiece()->getIcon() + " ";
          }
        }
        ++row_num;
        tmp += "| " + row_num_s + "\n";
        s += tmp;
      }
    }
    s += "  +---+---+---+---+---+---+---+---+\n";
    s += "    A   B   C   D   E   F   G   H\n";
    std::cout << s << std::endl;
  }

  void printBoardReverse() {
    std::string s = "    A   B   C   D   E   F   G   H\n";
    int row_num = 0;
    for (int i = 0; i < 16; ++i) {
      if (i % 2 == 0) {
        s += "  +---+---+---+---+---+---+---+---+\n";
      } else {
        std::string row_num_s = std::to_string(row_num + 1);
        std::string tmp = "";
        tmp += row_num_s + " ";
        for (int j = 0; j < 8; ++j) {
          tmp += "| ";
          if (this->board->at(7 - row_num).at(j)->getPiece() == nullptr) {
            tmp += this->board->at(7 - row_num).at(j)->getPrintStatus() + " ";
          } else {
            tmp += board->at(7 - row_num).at(j)->getPiece()->getIcon() + " ";
          }
        }
        ++row_num;
        tmp += "| " + row_num_s + "\n";
        s += tmp;
      }
    }
    s += "  +---+---+---+---+---+---+---+---+\n";
    s += "    A   B   C   D   E   F   G   H\n";
    std::cout << s << std::endl;
  }

  std::unordered_map<std::string, std::string> *getOccupiedCells() {
    return this->occupiedCells;
  }
  int getHalfMove(){
    return this->half_move;
  }
  void findOccupiedCells() {
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        if (board->at(i).at(j)->getPiece() == nullptr) {
          continue;
        } else {
          Cell *c = board->at(i).at(j);
          (*occupiedCells)[c->getPiece()->getPosition()] =
              c->getPiece()->getTeam();
        }
      }
    }
  }

  void generateAllMoves() {
    checkCastleStatus();
    this->whiteTeamAvailableMoves->clear();
    this->blackTeamAvailableMoves->clear();
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        if (board->at(i).at(j)->getPiece() == nullptr) {
          continue;
        } else {
          Chesspiece *c = board->at(i).at(j)->getPiece();
          c->removeAllMoves();
          c->generateMoves((*this->occupiedCells));
          std::vector<std::string> moves = c->getAvailableMoves();
          if (c->getTeam() == "white") {
            for (int i = 0; i < moves.size(); ++i) {
              this->whiteTeamAvailableMoves->insert(moves.at(i));
            }
          } else if (c->getTeam() == "black") {
            for (int i = 0; i < moves.size(); ++i) {
              this->blackTeamAvailableMoves->insert(moves.at(i));
            }
          }
        }
      }
    }
  }

  void generateWhiteTeamMoves() {
    this->whiteTeamAvailableMoves->clear();
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        if (board->at(i).at(j)->getPiece() == nullptr) {
          continue;
        } else if (board->at(i).at(j)->getPiece()->getTeam() == "white") {
          Chesspiece *c = board->at(i).at(j)->getPiece();
          c->removeAllMoves();
          c->generateMoves((*this->occupiedCells));
          std::vector<std::string> moves = c->getAvailableMoves();
          if (c->getTeam() == "white") {
            for (int i = 0; i < moves.size(); ++i) {
              this->whiteTeamAvailableMoves->insert(moves.at(i));
            }
          }
        }
      }
    }
  }

  void generateBlackTeamMoves() {
    this->blackTeamAvailableMoves->clear();
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        if (board->at(i).at(j)->getPiece() == nullptr) {
          continue;
        } else if (board->at(i).at(j)->getPiece()->getTeam() == "black") {
          Chesspiece *c = board->at(i).at(j)->getPiece();
          c->removeAllMoves();
          c->generateMoves((*this->occupiedCells));
          std::vector<std::string> moves = c->getAvailableMoves();
          if (c->getTeam() == "black") {
            for (int i = 0; i < moves.size(); ++i) {
              this->blackTeamAvailableMoves->insert(moves.at(i));
            }
          }
        }
      }
    }
  }

  Cell *getCell(std::string position) {
    char col_name = toupper(position.at(0));
    char row_name = position.at(1);
    int col = int(col_name) - 65;
    int row = 7 - (int(row_name) - 49);
    return this->board->at(row).at(col);
  }

  int movePiece(Cell *starting_location, Cell *destination) {
    strcpy(this->fen_en_passant_location, "-");
    std::string starting_location_name = starting_location->getName();
    std::string destination_name = destination->getName();
    std::vector<std::string> available_moves =
        starting_location->getPiece()->getAvailableMoves();
    bool canMove = false;
    int points = 0;
    this->half_move++;
    for (int i = 0; i < available_moves.size(); ++i) {
      if (available_moves.at(i) == destination_name) {
        canMove = true;
        break;
      }
    }
    if (canMove) {
      char start_col_name = toupper(starting_location_name.at(0));
      char start_row_name = starting_location_name.at(1);
      int start_col = int(start_col_name) - 65;
      int start_row = 7 - (int(start_row_name) - 49);

      char des_col_name = toupper(destination_name.at(0));
      char des_row_name = destination_name.at(1);
      int des_col = int(des_col_name) - 65;
      int des_row = 7 - (int(des_row_name) - 49);

      Chesspiece *tmp = starting_location->getPiece();
      tmp->setPosition(destination_name);
      if (tmp->getTeam() == "black"){
        this->full_move++;
      }
      if (tmp->getName().find("Pawn") != std::string::npos){
        this->half_move = 0;
        if (abs(start_row - des_row) == 2){
          char tmp_start_row = '\0';
          if (tmp->getTeam() == "white" && this->enPassantAvailable == "black"){
            tmp_start_row = char(49 + 2);
            this->fen_en_passant_location[0] = tolower(des_col_name);
            this->fen_en_passant_location[1] = tmp_start_row;
            this->fen_en_passant_location[2] = '\0';
          }
          else if (tmp->getTeam() == "black" && this->enPassantAvailable == "white"){
            tmp_start_row = char(49 + 5);
            this->fen_en_passant_location[0] = tolower(des_col_name);
            this->fen_en_passant_location[1] = tmp_start_row;
            this->fen_en_passant_location[2] = '\0';
          }
        }
      }
      this->board->at(start_row).at(start_col)->setPiece(nullptr);
      if (tmp->getName().find("King") != std::string::npos) {
        King *k = dynamic_cast<King *>(tmp);
        if (k->getTeam() == "white" && starting_location_name == "E1" &&
            destination_name == "C1" && k->getCastleLeft()) {
          Chesspiece *tmpr = getCell("A1")->getPiece();
          Rook *r = dynamic_cast<Rook *>(tmpr);
          Cell *c1 = getCell("A1");
          Cell *c2 = getCell("D1");
          movePiece(c1, c2);
        }
        if (k->getTeam() == "white" && starting_location_name == "E1" &&
            destination_name == "G1" && k->getCastleRight()) {
          Chesspiece *tmpr = getCell("H1")->getPiece();
          Rook *r = dynamic_cast<Rook *>(tmpr);
          Cell *c1 = getCell("H1");
          Cell *c2 = getCell("F1");
          movePiece(c1, c2);
        }
        if (k->getTeam() == "black" && starting_location_name == "E8" &&
            destination_name == "C8" && k->getCastleLeft()) {
          Chesspiece *tmpr = getCell("A8")->getPiece();
          Rook *r = dynamic_cast<Rook *>(tmpr);
          Cell *c1 = getCell("A8");
          Cell *c2 = getCell("D8");
          movePiece(c1, c2);
        }
        if (k->getTeam() == "black" && starting_location_name == "E8" &&
            destination_name == "G8" && k->getCastleRight()) {
          Chesspiece *tmpr = getCell("H8")->getPiece();
          Rook *r = dynamic_cast<Rook *>(tmpr);
          Cell *c1 = getCell("H8");
          Cell *c2 = getCell("F8");
          movePiece(c1, c2);
        }
        k->setMoved(true);
      } else if (tmp->getName().find("Rook") != std::string::npos) {
        Rook *r = dynamic_cast<Rook *>(tmp);
        r->setMoved(true);
      }
      if (destination->getPiece() != nullptr) {
        destination->getPiece()->setStatus("captured");
        points = destination->getPiece()->getPoints();
        this->half_move = 0;
      }
      if (tmp->getTeam() == "white" &&
          tmp->getName().find("Pawn") != std::string::npos &&
          destination_name.at(1) == '8') {
        this->promotionAvailable = true;
      } else if (tmp->getTeam() == "black" &&
                 tmp->getName().find("Pawn") != std::string::npos &&
                 destination_name.at(1) == '1') {
        this->promotionAvailable = true;
      }
      if (destination->getPiece() == nullptr) {
        char col_name = toupper(destination_name.at(0));
        char row_name = destination_name.at(1);
        int col = int(col_name) - 65;
        int row = 7 - (int(row_name) - 49);
        if (this->enPassantAvailable == "white") {
          if (tmp->getName().find("Pawn") != std::string::npos) {
            Pawn *p = dynamic_cast<Pawn *>(tmp);
            std::string enPassantTo = "";
            enPassantTo += enPassantPawn.at(0);
            enPassantTo += char(int(enPassantPawn.at(1)) + 1);
            int en_passant_column = int(enPassantPawn.at(0)) - 65;
            int en_passant_row = 7 - (int(enPassantPawn.at(1)) - 49);
            if (enPassantTo == destination_name) {
              Cell *c = getCell(enPassantPawn);
              points = c->getPiece()->getPoints();
              c->getPiece()->setStatus("captured");
              (*occupiedCells).erase(enPassantPawn);
              this->board->at(en_passant_row)
                  .at(en_passant_column)
                  ->setPiece(nullptr);
            }
          }
        }
        if (this->enPassantAvailable == "black") {
          if (tmp->getName().find("Pawn") != std::string::npos) {
            Pawn *p = dynamic_cast<Pawn *>(tmp);
            std::string enPassantTo = "";
            enPassantTo += enPassantPawn.at(0);
            enPassantTo += char(int(enPassantPawn.at(1)) - 1);
            int en_passant_column = int(enPassantPawn.at(0)) - 65;
            int en_passant_row = 7 - (int(enPassantPawn.at(1)) - 49);
            if (enPassantTo == destination_name) {
              Cell *c = getCell(enPassantPawn);
              points = c->getPiece()->getPoints();
              c->getPiece()->setStatus("captured");
              (*occupiedCells).erase(enPassantPawn);
              this->board->at(en_passant_row)
                  .at(en_passant_column)
                  ->setPiece(nullptr);
            }
          }
        }
        if (this->enPassantAvailable == "" && tmp->getTeam() == "white" &&
            tmp->getName().find("Pawn") != std::string::npos &&
            starting_location_name.at(1) == '2' &&
            destination_name.at(1) == '4') {
          if (col > 0) {
            std::string left_cell = "";
            left_cell += toupper(char(int(col_name) - 1));
            left_cell += row_name;
            Cell *c = getCell(left_cell);
            if (c->getPiece() != nullptr &&
                c->getPiece()->getName().find("Pawn") != std::string::npos &&
                c->getPiece()->getTeam() == "black") {
              Pawn *p = dynamic_cast<Pawn *>(c->getPiece());
              p->setEnPassant("right");
              this->enPassantAvailable = "black";
            }
          }
          if (col < 7) {
            std::string right_cell = "";
            right_cell += toupper((int(col_name) + 1));
            right_cell += row_name;
            Cell *c = getCell(right_cell);
            if (c->getPiece() != nullptr &&
                c->getPiece()->getName().find("Pawn") != std::string::npos &&
                c->getPiece()->getTeam() == "black") {
              Pawn *p = dynamic_cast<Pawn *>(c->getPiece());
              p->setEnPassant("left");
              this->enPassantAvailable = "black";
            }
          }
          this->enPassantPawn = destination_name;
        }
        if (this->enPassantAvailable == "" && tmp->getTeam() == "black" &&
            tmp->getName().find("Pawn") != std::string::npos &&
            starting_location_name.at(1) == '7' &&
            destination_name.at(1) == '5') {
          if (col > 0) {
            std::string left_cell = "";
            left_cell += toupper(char(int(col_name) - 1));
            left_cell += row_name;
            Cell *c = getCell(left_cell);
            if (c->getPiece() != nullptr &&
                c->getPiece()->getName().find("Pawn") != std::string::npos &&
                c->getPiece()->getTeam() == "white") {
              Pawn *p = dynamic_cast<Pawn *>(c->getPiece());
              p->setEnPassant("right");
              this->enPassantAvailable = "white";
            }
          }
          if (col < 7) {
            std::string right_cell = "";
            right_cell += toupper((int(col_name) + 1));
            right_cell += row_name;
            Cell *c = getCell(right_cell);
            if (c->getPiece() != nullptr &&
                c->getPiece()->getName().find("Pawn") != std::string::npos &&
                c->getPiece()->getTeam() == "white") {
              Pawn *p = dynamic_cast<Pawn *>(c->getPiece());
              p->setEnPassant("left");
              this->enPassantAvailable = "white";
            }
          }
          this->enPassantPawn = destination_name;
        }
      }

      this->board->at(des_row).at(des_col)->setPiece(tmp);
      (*occupiedCells).erase(starting_location_name);
      (*occupiedCells)[destination_name] = tmp->getTeam();
    }
    return points;
  }

  void promote(Team *team, Cell *destination) {
    std::vector<Chesspiece *> pieces = team->getPieces();
    Chesspiece *p = destination->getPiece();
    int i;
    for (i = 0; i < team->getPieces().size(); ++i) {
      if (team->getPieces().at(i)->getName() == p->getName()) {
        break;
      }
    }
    bool announced = false;
    while (true) {
      std::string input;
      if (!announced) {
        std::cout << "What do you want to promote your pawn to?\n";
        std::cout << "1. Queen\n";
        std::cout << "2. Rook\n";
        std::cout << "3. Bishop\n";
        std::cout << "4. Knight\n";
        std::cin >> input;
      }
      if (input == "1") {
        delete p;
        p = new Queen("Promoted " + team->getName() + " Queen", team->getName(),
                      destination->getName());
        pieces.at(i) = p;
        destination->setPiece(p);
        break;
      }
      if (input == "2") {
        delete p;
        p = new Rook("Promoted " + team->getName() + " Rook", team->getName(),
                     destination->getName());
        pieces.at(i) = p;
        destination->setPiece(p);
        break;
      }
      if (input == "3") {
        delete p;
        p = new Bishop("Promoted " + team->getName() + " Bishop",
                       team->getName(), destination->getName());
        pieces.at(i) = p;
        destination->setPiece(p);
        break;
      }
      if (input == "4") {
        delete p;
        p = new Knight("Promoted " + team->getName() + " Knight",
                       team->getName(), destination->getName());
        pieces.at(i) = p;
        destination->setPiece(p);
        break;
      }
    }
  }

  void showAvailableMoves(std::string position) {
    Cell *c = getCell(position);
    Chesspiece *p = c->getPiece();
    std::vector<std::string> moves = p->getAvailableMoves();
    for (int i = 0; i < moves.size(); i++) {
      std::string tmp_pos = moves.at(i);
      Cell *tmp_cell = getCell(tmp_pos);
      if (tmp_cell->getPiece() != nullptr) {
        if (tmp_cell->getPiece()->getName().find("King") == std::string::npos) {
          tmp_cell->getPiece()->setStatus("threatened");
        }
        tmp_cell->getPiece()->changeColor();
      } else {
        tmp_cell->setPrintStatus("*");
      }
    }
    if (enPassantAvailable != "") {
      if (p->getName().find("Pawn") != std::string::npos) {
        Cell *tmp_cell = getCell(enPassantPawn);
        Pawn *tmp_pawn = dynamic_cast<Pawn *>(p);
        if (tmp_pawn->getEnPassant() != "") {
          tmp_cell->getPiece()->setStatus("threatened");
          tmp_cell->getPiece()->changeColor();
        }
      }
    }
  }

  void removeAvailableMoves() {
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        Cell *c = this->board->at(i).at(j);
        if (c->getPrintStatus() == "*") {
          c->setPrintStatus(" ");
        }
        if (c->getPiece() != nullptr &&
            c->getPiece()->getStatus() != "active") {
          c->getPiece()->setStatus("active");
          c->getPiece()->changeColor();
        }
      }
    }
  }

  void showLegalMoves(std::set<std::string> legalMoves, int turn) {
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        Cell *c = this->board->at(i).at(j);
        if (legalMoves.find(c->getName()) != legalMoves.end()) {
          if (c->getPiece() != nullptr) {
            if (turn % 2 == 0 && c->getPiece()->getTeam() == "black") {
              c->getPiece()->setStatus("checking");
            } else if (turn % 2 != 0 && c->getPiece()->getTeam() == "white") {
              c->getPiece()->setStatus("checking");
            } else {
              c->getPiece()->setStatus("recommending");
            }
            c->getPiece()->changeColor();
          }
        } else {
          if (c->getPiece() == nullptr) {
            c->setPrintStatus(" ");
          }
        }
      }
    }
  }
  char *getFen(int turn){
    char fen[90] = "\0";
    for (int i = 0; i < 8; ++i){
      int empty_space = 0;
      for (int j = 0; j < 8; ++j){
        Cell *current_cell = this->board->at(i).at(j);
        if (current_cell->getPiece() != NULL){
          if (empty_space > 0){
            char num_buffer[2];
            sprintf(num_buffer, "%d", empty_space);
            strcat(fen, num_buffer);
            empty_space = 0;
          }
          char piece;
          if (current_cell->getPiece()->getName().find("Pawn") != std::string::npos){
            piece = 'P';
          }
          if (current_cell->getPiece()->getName().find("Rook") != std::string::npos){
            piece = 'R';
          }
          if (current_cell->getPiece()->getName().find("Knight") != std::string::npos){
            piece = 'N';
          }
          if (current_cell->getPiece()->getName().find("Bishop") != std::string::npos){
            piece = 'B';
          }
          if (current_cell->getPiece()->getName().find("Queen") != std::string::npos){
            piece = 'Q';
          }
          if (current_cell->getPiece()->getName().find("King") != std::string::npos){
            piece = 'K';
          }
          if (current_cell->getPiece()->getTeam() == "black"){
            piece = tolower(piece);
          }
          fen[strlen(fen)] = piece;
        }
        else if (current_cell->getPiece() == NULL){
          empty_space++;
          if (j == 7){
            char num_buffer[2];
            sprintf(num_buffer, "%d", empty_space);
            strcat(fen, num_buffer);
          }
        }
      }
      if (i < 7){
        strcat(fen,"/");
      }
    }
    if (turn % 2 == 0){ strcat(fen, " w");}
    else if (turn % 2 != 0){ strcat(fen, " b");}
    char castle_status[5] = "";
    if (!this->white->getKing()->getMoved()){
      if (this->board->at(7).at(7)->getPiece() != NULL && this->board->at(7).at(7)->getPiece()->getName().find("Rook") != std::string::npos){
        Rook *r = dynamic_cast<Rook*>(this->board->at(7).at(7)->getPiece());
        if (!r->getMoved()){
          strcat(castle_status, "K");
        }
      }
      if (this->board->at(7).at(0)->getPiece() != NULL && this->board->at(7).at(0)->getPiece()->getName().find("Rook") != std::string::npos){
        Rook *r = dynamic_cast<Rook*>(this->board->at(7).at(0)->getPiece());
        if (!r->getMoved()){
          strcat(castle_status, "Q");
        }
      }
    }
    if (!this->black->getKing()->getMoved()){
      if (this->board->at(0).at(7)->getPiece() != NULL && this->board->at(0).at(7)->getPiece()->getName().find("Rook") != std::string::npos){
        Rook *r = dynamic_cast<Rook*>(this->board->at(0).at(7)->getPiece());
        if (!r->getMoved()){
          strcat(castle_status, "k");
        }
      }
      if (this->board->at(0).at(0)->getPiece() != NULL && this->board->at(0).at(0)->getPiece()->getName().find("Rook") != std::string::npos){
        Rook *r = dynamic_cast<Rook*>(this->board->at(0).at(0)->getPiece());
        if (!r->getMoved()){
          strcat(castle_status, "q");
        }
      }
    }
    if (castle_status != ""){
      strcat(fen, " ");
      strcat(fen, castle_status);
    }
    strcat(fen, " ");
    strcat(fen, this->fen_en_passant_location);
    char fen_half_move[2];
    sprintf(fen_half_move, "%d", this->half_move);
    char fen_full_move[2];
    sprintf(fen_full_move, "%d", this->full_move);
    strcat(fen, " ");
    strcat(fen, fen_half_move);
    strcat(fen, " ");
    strcat(fen, fen_full_move);
    char* result = (char*) malloc(strlen(fen) + 1);
    strcpy(result, fen);
    return result;
  }
};

#endif