#include "Chesspiece.h"

#ifndef KNIGHT_H
#define KNIGHT_H

class Knight : public Chesspiece {
 public:
  Knight() {
    this->team = "";
    this->points = 0;
    this->status = "";
    this->position = "";
  }
  Knight(std::string name, std::string team, std::string position, int points = 3) {
    this->name = name;
    this->team = team;
    this->points = points;
    this->status = "active";
    this->position = position;
    if (team == "black") {
      this->icon = "\u2658";
    } else {
      this->icon = "\u265E";
    }
  }
  ~Knight(){};

  Knight(const Knight& other) {
    this->position = other.position;
    this->points = other.points;
    this->status = other.status;
    this->team = other.team;
    this->icon = other.icon;
    this->name = other.name;
    this->availableMoves = other.availableMoves;
  }

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
        this->icon = "\u2658";
      } else {
        this->icon = "\u265E";
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
    // move 2 cells down in an L shape
    if (row < 6) {
      if (col > 0) {
        std::string currentCell = "";
        currentCell += char(int(col_name) - 1);
        currentCell += char(int(row_name) - 2);
        if (occupiedCells.find(currentCell) == occupiedCells.end()) {
          this->availableMoves.push_back(currentCell);
        } else {
          if (this->team == "white" &&
              occupiedCells.find(currentCell)->second == "black") {
            this->availableMoves.push_back(currentCell);
          } else if (this->team == "black" &&
                     occupiedCells.find(currentCell)->second == "white") {
            this->availableMoves.push_back(currentCell);
          }
        }
      }
      if (col < 7) {
        std::string currentCell = "";
        currentCell += char(int(col_name) + 1);
        currentCell += char(int(row_name) - 2);
        if (occupiedCells.find(currentCell) == occupiedCells.end()) {
          this->availableMoves.push_back(currentCell);
        } else {
          if (this->team == "white" &&
              occupiedCells.find(currentCell)->second == "black") {
            this->availableMoves.push_back(currentCell);
          } else if (this->team == "black" &&
                     occupiedCells.find(currentCell)->second == "white") {
            this->availableMoves.push_back(currentCell);
          }
        }
      }
    }
    // move 2 cells up in an L shape
    if (row > 1) {
      if (col > 0) {
        std::string currentCell = "";
        currentCell += char(int(col_name) - 1);
        currentCell += char(int(row_name) + 2);
        if (occupiedCells.find(currentCell) == occupiedCells.end()) {
          this->availableMoves.push_back(currentCell);
        } else {
          if (this->team == "white" &&
              occupiedCells.find(currentCell)->second == "black") {
            this->availableMoves.push_back(currentCell);
          } else if (this->team == "black" &&
                     occupiedCells.find(currentCell)->second == "white") {
            this->availableMoves.push_back(currentCell);
          }
        }
      }
      if (col < 7) {
        std::string currentCell = "";
        currentCell += char(int(col_name) + 1);
        currentCell += char(int(row_name) + 2);
        if (occupiedCells.find(currentCell) == occupiedCells.end()) {
          this->availableMoves.push_back(currentCell);
        } else {
          if (this->team == "white" &&
              occupiedCells.find(currentCell)->second == "black") {
            this->availableMoves.push_back(currentCell);
          } else if (this->team == "black" &&
                     occupiedCells.find(currentCell)->second == "white") {
            this->availableMoves.push_back(currentCell);
          }
        }
      }
    }
    // move 2 cells to the right in an L shape
    if (col < 6) {
      if (row < 7) {
        std::string currentCell = "";
        currentCell += char(int(col_name) + 2);
        currentCell += char(int(row_name) - 1);
        if (occupiedCells.find(currentCell) == occupiedCells.end()) {
          this->availableMoves.push_back(currentCell);
        } else {
          if (this->team == "white" &&
              occupiedCells.find(currentCell)->second == "black") {
            this->availableMoves.push_back(currentCell);
          } else if (this->team == "black" &&
                     occupiedCells.find(currentCell)->second == "white") {
            this->availableMoves.push_back(currentCell);
          }
        }
      }
      if (row > 0) {
        std::string currentCell = "";
        currentCell += char(int(col_name) + 2);
        currentCell += char(int(row_name) + 1);
        if (occupiedCells.find(currentCell) == occupiedCells.end()) {
          this->availableMoves.push_back(currentCell);
        } else {
          if (this->team == "white" &&
              occupiedCells.find(currentCell)->second == "black") {
            this->availableMoves.push_back(currentCell);
          } else if (this->team == "black" &&
                     occupiedCells.find(currentCell)->second == "white") {
            this->availableMoves.push_back(currentCell);
          }
        }
      }
    }
    // move 2 cells to the left in an L shape
    if (col > 1) {
      if (row > 0) {
        std::string currentCell = "";
        currentCell += char(int(col_name) - 2);
        currentCell += char(int(row_name) + 1);
        if (occupiedCells.find(currentCell) == occupiedCells.end()) {
          this->availableMoves.push_back(currentCell);
        } else {
          if (this->team == "white" &&
              occupiedCells.find(currentCell)->second == "black") {
            this->availableMoves.push_back(currentCell);
          } else if (this->team == "black" &&
                     occupiedCells.find(currentCell)->second == "white") {
            this->availableMoves.push_back(currentCell);
          }
        }
      }
      if (row < 7) {
        std::string currentCell = "";
        currentCell += char(int(col_name) - 2);
        currentCell += char(int(row_name) - 1);
        if (occupiedCells.find(currentCell) == occupiedCells.end()) {
          this->availableMoves.push_back(currentCell);
        } else {
          if (this->team == "white" &&
              occupiedCells.find(currentCell)->second == "black") {
            this->availableMoves.push_back(currentCell);
          } else if (this->team == "black" &&
                     occupiedCells.find(currentCell)->second == "white") {
            this->availableMoves.push_back(currentCell);
          }
        }
      }
    }
  }
};

#endif