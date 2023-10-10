#include "Chesspiece.h"
#ifndef QUEEN_H 
#define QUEEN_H

class Queen : public Chesspiece{
    public:
        Queen(){
            this->team = "";
            this->points = 0;
            this->status = "";
            this->position = "";
        }
        Queen(std::string name, std::string team, std::string position, int points = 9){
            this->name = name;
            this->team = team;
            this->points = points;
            this->status = "active";
            this->position = position;
            if (team == "black"){
                this->icon = "\u2655";
            }
            else{
                this->icon = "\u265B";   
            }
        }
        ~Queen(){};
        
        Queen(const Queen& other) {
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
            else{
                if (team == "black"){
                    this->icon = "\u2655";
                }
                else{
                    this->icon = "\u265B";   
                }
                return false;
            }
        }
        void generateMoves(const std::unordered_map<std::string, std::string> occupiedCells) override{
            char col_name = toupper(this->position.at(0));
            char row_name = this->position.at(1);
            int col = int(col_name) - 65;
            int row = 7 - (int(row_name) - 49);
            //move to the right
            int i = row;
            int j = col;
            while (j < 7){
                std::string currentCell = "";
                currentCell += char(65+j+1);
                currentCell += char(49+(7-i));
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
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
            //move to the left
            i = row;
            j = col;
            while (j > 0){
                std::string currentCell = "";
                currentCell += char(65+j-1);
                currentCell += char(49+(7-i));
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
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
            
            //move to the top
            i = row;
            j = col;
            while (i > 0){
                std::string currentCell = "";
                currentCell += char(65+j);
                currentCell += char(49-i+8);
                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                    --i;
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
            //move to the bottom
            i = row;
            j = col;
            while (i < 7){
                std::string currentCell = "";
                currentCell += char(65+j);
                currentCell += char(49-i+6);

                if (occupiedCells.find(currentCell) == occupiedCells.end()){
                    this->availableMoves.push_back(currentCell);
                    ++i;
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
            //move to the bottom right
            i = row;
            j = col;
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