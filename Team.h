#include <string>
#include <vector>

#include "Chesspiece.h"
#include "Bishop.h"
#include "King.h"
#include "Knight.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"

#ifndef TEAM_H
#define TEAM_H

class Team {
 private:
  int points;
  std::vector<Chesspiece *> pieces;
  King *king;
  std::string name;

 public:
  Team(std::string name) {
    points = 0;
    king = nullptr;
    this->name = name;
    this->pieces = {};
  }
  ~Team() {
    for (int i = 0; i < pieces.size(); ++i) {
      delete pieces.at(i);
    }
  };
  Team(const Team &originalTeam) {
    // Copy the points
    this->points = originalTeam.points;
    // Copy the name
    this->name = originalTeam.name;

    // Deep copy the pieces
    for (Chesspiece *originalPiece : originalTeam.pieces) {
      Chesspiece *newPiece =
          nullptr;  // Assuming Chesspiece has a copy constructor
      if (typeid(*originalPiece) == typeid(Pawn)) {
        newPiece = new Pawn(*dynamic_cast<Pawn *>(originalPiece));
      } else if (typeid(*originalPiece) == typeid(King)) {
        newPiece = new King(*dynamic_cast<King *>(originalPiece));
        this->king = dynamic_cast<King *>(newPiece);
      } else if (typeid(*originalPiece) == typeid(Bishop)) {
        newPiece = new Bishop(*dynamic_cast<Bishop *>(originalPiece));
      } else if (typeid(*originalPiece) == typeid(Rook)) {
        newPiece = new Rook(*dynamic_cast<Rook *>(originalPiece));
      } else if (typeid(*originalPiece) == typeid(Queen)) {
        newPiece = new Queen(*dynamic_cast<Queen *>(originalPiece));
      } else if (typeid(*originalPiece) == typeid(Knight)) {
        newPiece = new Knight(*dynamic_cast<Knight *>(originalPiece));
      }
      this->pieces.push_back(newPiece);
    }
  }

  void addPiece(Chesspiece *piece) { this->pieces.push_back(piece); }

  int getPoints() const { return this->points; }

  // Setter for points
  void setPoints(int points) { this->points = points; }

  void addPoints(int points) { this->points += points; }

  // Getter for pieces
  std::vector<Chesspiece *> getPieces() const { return this->pieces; }

  // Setter for pieces
  void setPieces(std::vector<Chesspiece *> pieces) { this->pieces = pieces; }

  // Getter for king
  King *getKing() const { return this->king; }

  // Setter for king
  void setKing(King *king) { this->king = king; }

  std::string getName() { return this->name; }

  void setName(std::string name) { this->name = name; }
};

#endif