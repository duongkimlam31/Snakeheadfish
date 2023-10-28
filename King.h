#include "Chesspiece.h"

#ifndef KING_H 
#define KING_H

class King : public Chesspiece{
    private:
        bool moved;
        bool castle_left;
        bool castle_right;
    public:
        King(){
            this->team = "";
            this->points = 0;
            this->status = "";
            this->position = "";
            this->moved = false;
            this->castle_left = false;
            this->castle_right = false;
        }
        King(std::string name, std::string team, std::string position, int points = 0){
            this->name = name;
            this->team = team;
            this->points = points;
            this->status = "active";
            this->position = position;
            this->moved = false;
            this->castle_left = false;
            this->castle_right = false;
            if (team == "black"){
                    this->icon = "\u2654";
            }
            else{
                this->icon = "\u265A";   
            }
        }
        King(King& other) {
            this->position = other.position;
            this->points = other.points;
            this->status = other.status;
            this->team = other.team;
            this->icon = other.icon;
            this->name = other.name;
            this->moved = other.moved;
            this->castle_left = other.castle_left;
            this->castle_right = other.castle_right;
            this->availableMoves = other.availableMoves;
        } 

        void setMoved(bool moved){
            this->moved = moved;
        }

        void setCastleLeft(bool castle_left){
            this->castle_left = castle_left;
        }

        void setCastleRight(bool castle_right){
            this->castle_right = castle_right;
        }

        bool getMoved(){
            return this->moved;
        }

        bool getCastleLeft(){
            return this->castle_left;
        }

        bool getCastleRight(){
            return this->castle_right;
        }

        bool changeColor(){
            if (this->status == "checked"){
                this->icon = "\x1b[31m" + this->icon + "\x1b[0m";
                return true;
            }
            else if (this->status == "recommending"){
                this->icon = "\033[33m" + this->icon + "\033[0m";
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
            if (castle_left){
                std::string currentCell = "";
                currentCell += char(65+col-2);
                currentCell += char(49+(7-row));
                availableMoves.push_back(currentCell);
            }
            if (castle_right){
                std::string currentCell = "";
                currentCell += char(65+col+2);
                currentCell += char(49+(7-row));
                availableMoves.push_back(currentCell);
            }
        }
};

#endif