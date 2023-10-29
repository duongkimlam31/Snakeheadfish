#include "Chesspiece.h"
#ifndef PAWN_H
#define PAWN_H

class Pawn : public Chesspiece {
 private:
  bool can_move_two_cells;
  std::string en_passant;

 public:
  Pawn() {
    this->team = "";
    this->points = 0;
    this->status = "";
    this->position = "";
    this->can_move_two_cells = false;
    this->en_passant = "";
  }
  Pawn(std::string name, std::string team, std::string position,
       int points = 1) {
    this->name = name;
    this->team = team;
    this->points = points;
    this->status = "active";
    this->position = position;
    this->can_move_two_cells = true;
    this->en_passant = "";
    if (team == "black") {
      this->icon = "\u2659";
    } else {
      this->icon = "\u265F";
    }
  }
  ~Pawn(){};

  Pawn(Pawn& other) {
    this->position = other.position;
    this->points = other.points;
    this->status = other.status;
    this->team = other.team;
    this->icon = other.icon;
    this->name = other.name;
    this->availableMoves = other.availableMoves;
    this->can_move_two_cells = other.can_move_two_cells;
    this->en_passant = other.en_passant;
  }

  std::string getEnPassant() { return this->en_passant; }

  void setEnPassant(std::string enPassant) { this->en_passant = enPassant; }

  bool changeColor() {
    if (this->status == "threatened") {
      this->icon = "\x1b[31m" + this->icon + "\x1b[0m";
      return true;
    } else if (this->status == "checking") {
      this->icon = "\033[35m" + this->icon + "\033[0m";
      return true;
    } else if (this->status == "recommending") {
      this->icon = "\033[33m" + this->icon + "\033[0m";
      return true;
    } else {
      if (team == "black") {
        this->icon = "\u2659";
      } else {
        this->icon = "\u265F";
      }
      return false;
    }
  }
  void generateMoves(const std::unordered_map<std::string, std::string>
                         occupiedCells) override {
    char col_name = toupper(this->position.at(0));
    char row_name = this->position.at(1);
    int col = int(col_name) - 65;
    int row = 7 - (int(row_name) - 49);

    if (this->team == "white") {
      if (row_name != '2') {
        this->can_move_two_cells = false;
      }
      // move 1 cell up
      if (!can_move_two_cells && row > 0) {
        std::string currentCell = "";
        currentCell += col_name;
        currentCell += char(int(row_name) + 1);
        if (occupiedCells.find(currentCell) == occupiedCells.end()) {
          this->availableMoves.push_back(currentCell);
        }
      }
      // move 2 cells up
      else if (can_move_two_cells) {
        std::string currentCell = "";
        currentCell += col_name;
        currentCell += char(int(row_name) + 1);
        if (occupiedCells.find(currentCell) == occupiedCells.end() && row > 0) {
          this->availableMoves.push_back(currentCell);
          currentCell = "";
          currentCell += col_name;
          currentCell += char(int(row_name) + 2);
          if (occupiedCells.find(currentCell) == occupiedCells.end() &&
              row > 1) {
            this->availableMoves.push_back(currentCell);
          }
        }
      }
      if (row > 0 && col > 0) {
        std::string attackCell = "";
        attackCell += char(int(col_name) - 1);
        attackCell += char(int(row_name) + 1);
        if (occupiedCells.find(attackCell) != occupiedCells.end() &&
            occupiedCells.find(attackCell)->second == "black") {
          this->availableMoves.push_back(attackCell);
        }
      }
      if (row >= 0 && col < 7) {
        std::string attackCell = "";
        attackCell += char(int(col_name) + 1);
        attackCell += char(int(row_name) + 1);
        if (occupiedCells.find(attackCell) != occupiedCells.end() &&
            occupiedCells.find(attackCell)->second == "black") {
          this->availableMoves.push_back(attackCell);
        }
      }
      if (en_passant == "left") {
        std::string attackCell = "";
        attackCell += char(int(col_name) - 1);
        attackCell += char(int(row_name) + 1);
        if (occupiedCells.find(attackCell) == occupiedCells.end()) {
          this->availableMoves.push_back(attackCell);
        }
      } else if (en_passant == "right") {
        std::string attackCell = "";
        attackCell += char(int(col_name) + 1);
        attackCell += char(int(row_name) + 1);
        if (occupiedCells.find(attackCell) == occupiedCells.end()) {
          this->availableMoves.push_back(attackCell);
        }
      }
    } else if (this->team == "black") {
      if (row_name != '7') {
        this->can_move_two_cells = false;
      }
      // move 1 cell down
      if (!can_move_two_cells && row < 7) {
        std::string currentCell = "";
        currentCell += col_name;
        currentCell += char(int(row_name) - 1);
        if (occupiedCells.find(currentCell) == occupiedCells.end()) {
          this->availableMoves.push_back(currentCell);
        }
      }
      // move 2 cells down
      else if (can_move_two_cells) {
        std::string currentCell = "";
        currentCell += col_name;
        currentCell += char(int(row_name) - 1);
        if (occupiedCells.find(currentCell) == occupiedCells.end() && row < 7) {
          this->availableMoves.push_back(currentCell);
          currentCell = "";
          currentCell += col_name;
          currentCell += char(int(row_name) - 2);
          if (occupiedCells.find(currentCell) == occupiedCells.end() &&
              row < 6) {
            this->availableMoves.push_back(currentCell);
          }
        }
      }
      if (row < 7 && col <= 7) {
        std::string attackCell = "";
        attackCell += char(int(col_name) - 1);
        attackCell += char(int(row_name) - 1);
        if (occupiedCells.find(attackCell) != occupiedCells.end() &&
            occupiedCells.find(attackCell)->second == "white") {
          this->availableMoves.push_back(attackCell);
        }
      }
      if (row < 7 && col >= 0) {
        std::string attackCell = "";
        attackCell += char(int(col_name) + 1);
        attackCell += char(int(row_name) - 1);
        if (occupiedCells.find(attackCell) != occupiedCells.end() &&
            occupiedCells.find(attackCell)->second == "white") {
          this->availableMoves.push_back(attackCell);
        }
      }
      if (en_passant == "left") {
        std::string attackCell = "";
        attackCell += char(int(col_name) - 1);
        attackCell += char(int(row_name) - 1);
        if (occupiedCells.find(attackCell) == occupiedCells.end()) {
          this->availableMoves.push_back(attackCell);
        }
      } else if (en_passant == "right") {
        std::string attackCell = "";
        attackCell += char(int(col_name) + 1);
        attackCell += char(int(row_name) - 1);
        if (occupiedCells.find(attackCell) == occupiedCells.end()) {
          this->availableMoves.push_back(attackCell);
        }
      }
    }
  }
};

#endif