#include "Chesspiece.h"

#ifndef ROOK_H
#define ROOK_H

class Rook : public Chesspiece {
 private:
  bool moved;
  std::string castle_location;

 public:
  Rook() {
    this->team = "";
    this->points = 0;
    this->status = "";
    this->position = "";
    this->moved = false;
    this->castle_location = "";
  }
  Rook(std::string name, std::string team, std::string position,
       int points = 5) {
    this->name = name;
    this->team = team;
    this->points = points;
    this->status = "active";
    this->position = position;
    this->moved = false;
    this->castle_location = "";
    if (team == "black") {
      this->icon = "\u2656";
    } else {
      this->icon = "\u265C";
    }
  }
  ~Rook(){};
  Rook(const Rook& other) {
    this->position = other.position;
    this->points = other.points;
    this->status = other.status;
    this->team = other.team;
    this->icon = other.icon;
    this->name = other.name;
    this->moved = other.moved;
    this->availableMoves = other.availableMoves;
  }
  void setMoved(bool moved) { this->moved = moved; }

  void setCastleLocation(std::string castle_location) {
    this->castle_location = castle_location;
  }

  bool getMoved() { return this->moved; }

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
        this->icon = "\u2656";
      } else {
        this->icon = "\u265C";
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

    // move to the bottom
    int i = row;
    int j = col;
    while (i < 7) {
      std::string currentCell = "";
      currentCell += char(65 + j);
      currentCell += char(49 - i + 6);

      if (occupiedCells.find(currentCell) == occupiedCells.end()) {
        this->availableMoves.push_back(currentCell);
        ++i;
      } else {
        if (this->team == "white" &&
            occupiedCells.find(currentCell)->second == "black") {
          this->availableMoves.push_back(currentCell);
        } else if (this->team == "black" &&
                   occupiedCells.find(currentCell)->second == "white") {
          this->availableMoves.push_back(currentCell);
        }
        break;
      }
    }
    // move to the top
    i = row;
    j = col;
    while (i > 0) {
      std::string currentCell = "";
      currentCell += char(65 + j);
      currentCell += char(49 - i + 8);
      if (occupiedCells.find(currentCell) == occupiedCells.end()) {
        this->availableMoves.push_back(currentCell);
        --i;
      } else {
        if (this->team == "white" &&
            occupiedCells.find(currentCell)->second == "black") {
          this->availableMoves.push_back(currentCell);
        } else if (this->team == "black" &&
                   occupiedCells.find(currentCell)->second == "white") {
          this->availableMoves.push_back(currentCell);
        }
        break;
      }
    }
    // move to the right
    i = row;
    j = col;
    while (j < 7) {
      std::string currentCell = "";
      currentCell += char(65 + j + 1);
      currentCell += char(49 + (7 - i));
      if (occupiedCells.find(currentCell) == occupiedCells.end()) {
        this->availableMoves.push_back(currentCell);
        ++j;
      } else {
        if (this->team == "white" &&
            occupiedCells.find(currentCell)->second == "black") {
          this->availableMoves.push_back(currentCell);
        } else if (this->team == "black" &&
                   occupiedCells.find(currentCell)->second == "white") {
          this->availableMoves.push_back(currentCell);
        }
        break;
      }
    }
    // move to the left
    i = row;
    j = col;
    while (j > 0) {
      std::string currentCell = "";
      currentCell += char(65 + j - 1);
      currentCell += char(49 + (7 - i));
      if (occupiedCells.find(currentCell) == occupiedCells.end()) {
        this->availableMoves.push_back(currentCell);
        --j;
      } else {
        if (this->team == "white" &&
            occupiedCells.find(currentCell)->second == "black") {
          this->availableMoves.push_back(currentCell);
        } else if (this->team == "black" &&
                   occupiedCells.find(currentCell)->second == "white") {
          this->availableMoves.push_back(currentCell);
        }
        break;
      }
    }
    if (castle_location != "" && !moved) {
      this->availableMoves.push_back(castle_location);
    }
  }
};

#endif