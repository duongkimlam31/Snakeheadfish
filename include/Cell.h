#ifndef CELL_H
#define CELL_H
#include <string>
#include <typeinfo>

#include "Bishop.h"
#include "Chesspiece.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"

class Cell {
 private:
  std::string name;
  Chesspiece* piece;
  std::string printStatus;

 public:
  Cell() {
    this->name = "";
    this->piece = nullptr;
    this->printStatus = "";
  }
  Cell(std::string name, Chesspiece* piece = nullptr) {
    this->name = name;
    this->piece = piece;
    this->printStatus = " ";
  }

  // Getter for name
  std::string getName() const { return this->name; }

  // Setter for name
  void setName(const std::string name) { this->name = name; }

  // Getter for piece
  Chesspiece* getPiece() const { return this->piece; }

  // Setter for piece
  void setPiece(Chesspiece* piece) { this->piece = piece; }

  std::string getPrintStatus() const { return this->printStatus; }

  // Setter for name
  void setPrintStatus(const std::string printStatus) {
    this->printStatus = printStatus;
  }
};

#endif
