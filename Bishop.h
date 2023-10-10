#include "Chesspiece.h"

#ifndef BISHOP_H 
#define BISHOP_H

class Bishop : public Chesspiece{
    public:
        Bishop(){
            this->team = "";
            this->points = 0;
            this->status = "";
            this->position = "";
            this->name = "";
        }
        Bishop(std::string name, std::string team, std::string position, int points = 3){
            this->name = name;
            this->team = team;
            this->points = points;
            this->status = "active";
            this->position = position;
            if (team == "black"){
                this->icon = "\u2657";
            }
            else{
                this->icon = "\u265D";   
            }
        }
        ~Bishop(){};

        Bishop(const Bishop& other) {
            this->position = other.position;
            this->points = other.points;
            this->status = other.status;
            this->team = other.team;
            this->icon = other.icon;
            this->name = other.name;
            this->availableMoves = other.availableMoves;
        }
        
        bool changeColor(){
            if (this->status == "threatened"){
                this->icon = "\x1b[31m" + this->icon + "\x1b[0m";
                return true;
            }
            else if (this->status == "checking"){
                this->icon = "\033[35m" + this->icon + "\033[0m";
                return true;
            }
            else if (this->status == "recommending"){
                this->icon = "\033[33m" + this->icon + "\033[0m";
                return true;
            }
            else{
                if (team == "black"){
                    this->icon = "\u2657";
                }
                else{
                    this->icon = "\u265D";   
                }
                return false;
            }
        }
        void generateMoves(const std::unordered_map<std::string, std::string> occupiedCells) override {
            char col_name = toupper(this->position.at(0));
            char row_name = this->position.at(1);
            int col = int(col_name) - 65;
            int row = 7 - (int(row_name) - 49);

            //move to the bottom right
            int i = row;
            int j = col;
            while (i < 7 && j < 7){
                std::string currentCell = "";
                currentCell += char(65+j+1);
                currentCell += char(49-i+6);

                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                    ++i;
                    ++j;
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                    break;
                }
            }
            //move to the bottom left
            i = row;
            j = col;
            while (i < 7 && j > 0){
                std::string currentCell = "";
                currentCell = "";
                currentCell += char(65+j-1);
                currentCell += char(49-i+6);
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                    ++i;
                    --j;
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                    break;
                }
            }
            //move to the top right
            i = row;
            j = col;
            while (i > 0 && j < 7){
                std::string currentCell = "";
                currentCell += char(65+j+1);
                currentCell += char(49-i+8);
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                    --i;
                    ++j;
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                    break;
                }
            }
            //move to the top left
            i = row;
            j = col;
            while (i > 0 && j > 0){
                std::string currentCell = "";
                currentCell += char(65+j-1);
                currentCell += char(49-i+8);
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                    --i;
                    --j;
                }
                else{
                    if (this->team == "white" && occupiedCells.find(currentCell)->second == "black"){
                        this->availableMoves.push_back(currentCell);
                    }
                    else if (this->team == "black" && occupiedCells.find(currentCell)->second == "white"){
                        this->availableMoves.push_back(currentCell);
                    }
                    break;
                }
            }
        }
};

#endif