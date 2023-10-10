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

    Cell(const Cell& other){
        this->name = other.name;
        this->printStatus = other.printStatus;
        if (other.piece == nullptr){
            this->piece = nullptr;
        }
        else if (typeid(*other.piece) == typeid(Pawn)){
            this->piece = new Pawn(*dynamic_cast<Pawn*>(other.getPiece()));
        }
        else if (typeid(*other.piece) == typeid(King)){
            this->piece = new King(*dynamic_cast<King*>(other.getPiece()));
        }
        else if (typeid(*other.piece) == typeid(Bishop)){
            this->piece = new Bishop(*dynamic_cast<Bishop*>(other.getPiece()));
        }
        else if (typeid(*other.piece) == typeid(Rook)){
            this->piece = new Rook(*dynamic_cast<Rook*>(other.getPiece()));
        }
        else if (typeid(*other.piece) == typeid(Queen)){
            this->piece = new Queen(*dynamic_cast<Queen*>(other.getPiece()));
        }
        else if (typeid(*other.piece) == typeid(Knight)){
            this->piece = new Knight(*dynamic_cast<Knight*>(other.getPiece()));
        }
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
