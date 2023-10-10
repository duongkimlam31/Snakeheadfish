#include <vector>
#include <string>
#include "Chesspiece.h"

#ifndef TEAM_H 
#define TEAM_H

class Team{
    private:
        int points;
        std::vector<Chesspiece*> pieces;
        Chesspiece *king;
        std::string name;

    public:
        Team(std::string name){
            points = 0;
            king = nullptr;
            this->name = name;
        }
        ~Team(){};
        void addPiece(Chesspiece *piece){
            this->pieces.push_back(piece);
        }

        int getPoints() const {
            return this->points;
        }

        // Setter for points
        void setPoints(int points) {
            this->points = points;
        }

        void addPoints(int points){
            this->points += points;
        }

        // Getter for pieces
        std::vector<Chesspiece*> getPieces() const {
            return this->pieces;
        }

        // Setter for pieces
        void setPieces(std::vector<Chesspiece*> pieces) {
            this->pieces = pieces;
        }
        
        // Getter for king
        Chesspiece *getKing() const{
            return this->king;
        }

        // Setter for king
        void setKing(Chesspiece *king){
            this->king = king;
        }

        std::string getName(){
            return this->name;
        }

        void setName(std::string name){
            this->name = name;
        }
};



#endif