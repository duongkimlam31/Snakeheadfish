#include "Chesspiece.h"
#include <string>
#include <typeinfo>
#include "Pawn.h"
#include "Queen.h"
#include "King.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"

#ifndef CELL_H
#define CELL_H

class Cell {
private:
    std::string name;
    Chesspiece* piece;
    std::string printStatus;

public:
    Cell(){
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
    std::string getName() const {
        return this->name;
    }

    // Setter for name
    void setName(const std::string name) {
        this->name = name;
    }

    // Getter for piece
    Chesspiece* getPiece() const {
        return this->piece;
    }

    // Setter for piece
    void setPiece(Chesspiece* piece) {
        this->piece = piece;
    }

    std::string getPrintStatus() const {
        return this->printStatus;
    }

    // Setter for name
    void setPrintStatus(const std::string printStatus) {
        this->printStatus = printStatus;
    }
};

#endif
