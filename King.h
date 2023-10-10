#include "Chesspiece.h"

#ifndef KING_H 
#define KING_H

class King : public Chesspiece{
    public:
        King(){
            this->team = "";
            this->points = 0;
            this->status = "";
            this->position = "";
        }
        King(std::string name, std::string team, std::string position, int points = 0){
            this->name = name;
            this->team = team;
            this->points = points;
            this->status = "active";
            this->position = position;
            if (team == "black"){
                    this->icon = "\u2654";
            }
            else{
                this->icon = "\u265A";   
            }
        }
        bool changeColor(){
            if (this->status == "checked"){
                this->icon = "\x1b[31m" + this->icon + "\x1b[0m";
                return true;
            }
            else{
                if (team == "black"){
                        this->icon = "\u2654";
                }
                else{
                    this->icon = "\u265A";   
                }
                return false;
            }
        }
        ~King(){};

        King(const King& other) {
            this->position = other.position;
            this->points = other.points;
            this->status = other.status;
            this->team = other.team;
            this->icon = other.icon;
            this->name = other.name;
            this->availableMoves = other.availableMoves;
        }

        void generateMoves(const std::unordered_map<std::string, std::string> occupiedCells) override{
            char col_name = toupper(this->position.at(0));
            char row_name = this->position.at(1);
            int col = int(col_name) - 65;
            int row = 7 - (int(row_name) - 49);
            // move right
            if (col < 7){
                std::string currentCell = "";
                currentCell += char(65+col+1);
                currentCell += char(49+(7-row));
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                }
            }
            //move left
            if (col > 0){
                std::string currentCell = "";
                currentCell += char(65+col-1);
                currentCell += char(49+(7-row));
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                }
            }
            //move down
            if (row < 7){
                std::string currentCell = "";
                currentCell += char(65+col);
                currentCell += char(49-row+6);
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                }
            }
            //move up
            if (row > 0){
                std::string currentCell = "";
                currentCell += char(65+col);
                currentCell += char(49-row+8);
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                }
            }
            //move to the top right
            if (col < 7 && row > 0){
                std::string currentCell = "";
                currentCell += char(65+col+1);
                currentCell += char(49-row+8);
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                }
            }
            //move to the top left
            if (col > 0 && row > 0){
                std::string currentCell = "";
                currentCell += char(65+col-1);
                currentCell += char(49-row+8);
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                }
            }
            //move to the bottom right
            if (col < 7 && row < 7){
                std::string currentCell = "";
                currentCell += char(65+col+1);
                currentCell += char(49-row+6);
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                }
            }
            //move to the bottom left
            if (col > 0 && row < 7){
                std::string currentCell = "";
                currentCell += char(65+col-1);
                currentCell += char(49-row+6);
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                }
            }
        }
};

#endif