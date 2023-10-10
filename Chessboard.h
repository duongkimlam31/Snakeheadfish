#include "Chesspiece.h"
#include "Cell.h"
#include <string.h>
#include <vector>
#include <unordered_map>
#include <set>
#include <iostream>
#include "Queen.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "cstring"

#ifndef CHESSBOARD_H
#define CHESSBOARD_H

class Chessboard{
    private:
        std::vector<std::vector<Cell*>> *board = new std::vector<std::vector<Cell*>>;
        std::unordered_map<std::string, std::string> *occupiedCells = new std::unordered_map<std::string, std::string>;
        std::set<std::string> *whiteTeamAvailableMoves = new std::set<std::string>;
        std::set<std::string> *blackTeamAvailableMoves = new std::set<std::string>;
        bool promotionAvailable;

    public:
        Chessboard(){
            for (int i = 0; i < 8; ++i){
                std::string row = std::to_string(8-i);
                board->push_back({});
                for (int j = 0; j < 8; ++j){
                    char col = char(65+j);
                    std::string cell_name = col+row; 
                    Cell *c = new Cell(cell_name);
                    board->at(i).push_back(c);
                }
            }
            promotionAvailable = false;
        }
        ~Chessboard() {
            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < 8; ++j) {
                    delete board->at(i).at(j);
                }
            }
            delete occupiedCells;
            delete whiteTeamAvailableMoves;
            delete blackTeamAvailableMoves;
        }
            
       Chessboard& operator=(const Chessboard& origObject) {
            if (this != &origObject) {
                // Delete the existing resources
                for (std::vector<Cell*>& row : *board) {
                    for (Cell* cell : row) {
                        delete cell;
                    }
                }
                delete this->board;
                delete this->occupiedCells;
                delete this->whiteTeamAvailableMoves;
                delete this->blackTeamAvailableMoves;

                // Create new resources and deep copy the contents
                this->board = new std::vector<std::vector<Cell*>>;
                for (const std::vector<Cell*>& origRow : *(origObject.board)) {
                    std::vector<Cell*> newRow;
                    for (Cell* cell : origRow) {
                        Cell *c = new Cell(*cell);
                        newRow.push_back(c); // Deep copy each cell
                    }
                    this->board->push_back(newRow);
                }

                occupiedCells = new std::unordered_map<std::string, std::string>(*origObject.occupiedCells);
                whiteTeamAvailableMoves = new std::set<std::string>(*origObject.whiteTeamAvailableMoves);
                blackTeamAvailableMoves = new std::set<std::string>(*origObject.blackTeamAvailableMoves);
            }
            return *this;
        }
            
        std::vector<std::vector<Cell*>> *getBoard(){
            return this->board;
        }
        std::set<std::string> *getWhiteTeamAvailableMoves(){
            return this->whiteTeamAvailableMoves;
        }
        std::set<std::string> *getBlackTeamAvailableMoves(){
            return this->blackTeamAvailableMoves;
        }
        bool getPromotionAvailable(){
            return this->promotionAvailable;
        }
        void setPromotionAvailable(bool promotionAvailable){
            this->promotionAvailable = promotionAvailable;
        }

        void setBoard(std::vector<std::vector<Cell*>> *board){
            this->board = board;
        }
        void printBoard(){
            std::string s = "    A   B   C   D   E   F   G   H\n";
            int row_num = 0;
            for (int i = 0; i < 16; ++i){
                if (i % 2 == 0){
                    s += "  +---+---+---+---+---+---+---+---+\n";
                }
                else{
                    std::string row_num_s = std::to_string(8-row_num);
                    std::string tmp = "";
                    tmp += row_num_s + " ";
                    for (int j = 0; j < 8; ++j){
                        tmp += "| ";
                        if (this->board->at(row_num).at(j)->getPiece() == nullptr){
                            tmp += this->board->at(row_num).at(j)->getPrintStatus() + " ";
                        }
                        else{
                            tmp += board->at(row_num).at(j)->getPiece()->getIcon() + " ";
                        }
                    }
                    ++row_num;
                    tmp += "| " + row_num_s + "\n";
                    s += tmp;
                }
            }
            s += "  +---+---+---+---+---+---+---+---+\n";
            s += "    A   B   C   D   E   F   G   H\n";
            std::cout << s << std::endl;
        }
        std::unordered_map<std::string, std::string> *getOccupiedCells(){
            return this->occupiedCells;
        }
        void findOccupiedCells(){
            for (int i = 0; i < 8; ++i){
                for (int j = 0; j < 8; ++j){
                    if (board->at(i).at(j)->getPiece() == nullptr){
                        continue;
                    }
                    else{
                        Cell *c = board->at(i).at(j);
                        (*occupiedCells)[c->getPiece()->getPosition()] = c->getPiece()->getTeam();
                    }
                }
            }
        }

        void generateAllMoves(){
            this->whiteTeamAvailableMoves->clear();
            this->blackTeamAvailableMoves->clear();
            for (int i = 0; i < 8; ++i){
                for (int j = 0; j < 8; ++j){
                    if (board->at(i).at(j)->getPiece() == nullptr){
                        continue;
                    }
                    else{
                        Chesspiece *c = board->at(i).at(j)->getPiece();
                        c->removeAllMoves();
                        c->generateMoves((*this->occupiedCells));
                        std::vector<std::string> moves = c->getAvailableMoves();
                        if (c->getTeam() == "white"){
                            for(int i = 0; i < moves.size(); ++i){
                                this->whiteTeamAvailableMoves->insert(moves.at(i));
                            }
                        }
                        else if (c->getTeam() == "black"){
                            for(int i = 0; i < moves.size(); ++i){
                                this->blackTeamAvailableMoves->insert(moves.at(i));
                            }
                        }
                    }
                }
            }
        }

        void generateWhiteTeamMoves(){
            this->whiteTeamAvailableMoves->clear();
            for (int i = 0; i < 8; ++i){
                for (int j = 0; j < 8; ++j){
                    if (board->at(i).at(j)->getPiece() == nullptr){
                        continue;
                    }
                    else if (board->at(i).at(j)->getPiece()->getTeam() == "white"){
                        Chesspiece *c = board->at(i).at(j)->getPiece();
                        c->removeAllMoves();
                        c->generateMoves((*this->occupiedCells));
                        std::vector<std::string> moves = c->getAvailableMoves();
                        if (c->getTeam() == "white"){
                            for(int i = 0; i < moves.size(); ++i){
                                this->whiteTeamAvailableMoves->insert(moves.at(i));
                            }
                        }
                    }
                }
            }
        }

        void generateBlackTeamMoves(){
            this->blackTeamAvailableMoves->clear();
            for (int i = 0; i < 8; ++i){
                for (int j = 0; j < 8; ++j){
                    if (board->at(i).at(j)->getPiece() == nullptr){
                        continue;
                    }
                    else if (board->at(i).at(j)->getPiece()->getTeam() == "black"){
                        Chesspiece *c = board->at(i).at(j)->getPiece();
                        c->removeAllMoves();
                        c->generateMoves((*this->occupiedCells));
                        std::vector<std::string> moves = c->getAvailableMoves();
                        if (c->getTeam() == "black"){
                            for(int i = 0; i < moves.size(); ++i){
                                this->blackTeamAvailableMoves->insert(moves.at(i));
                            }
                        }
                    }
                }
            }
        }

        Cell *getCell(std::string position){
            char col_name = toupper(position.at(0));
            char row_name = position.at(1);
            int col = int(col_name) - 65;
            int row = 7 - (int(row_name) - 49);
            return this->board->at(row).at(col);
        }

        void movePiece(Cell *starting_location, Cell *destination){
            std::string destination_name = destination->getName();
            std::string starting_location_name = starting_location->getName();
            std::vector<std::string> available_moves = starting_location->getPiece()->getAvailableMoves();
            bool canMove = false;
            for (int i = 0; i < available_moves.size(); ++i){
                if (available_moves.at(i) == destination_name){
                    canMove = true;
                    break;
                }
            }
            if (canMove){
                char start_col_name = toupper(starting_location_name.at(0));
                char start_row_name = starting_location_name.at(1);
                int start_col = int(start_col_name) - 65;
                int start_row = 7 - (int(start_row_name) - 49);

                char des_col_name = toupper(destination_name.at(0));
                char des_row_name = destination_name.at(1);
                int des_col = int(des_col_name) - 65;
                int des_row = 7 - (int(des_row_name) - 49);

                Chesspiece *tmp = starting_location->getPiece();
                tmp->setPosition(destination_name);

                this->board->at(start_row).at(start_col)->setPiece(nullptr);
                if (destination->getPiece() != nullptr){
                    destination->getPiece()->setStatus("captured");
                    if (tmp->getTeam() == "white" && tmp->getName().find("Pawn") != std::string::npos && destination_name.at(1) == '8'){
                        this->promotionAvailable = true;
                    }
                    else if (tmp->getTeam() == "black" && tmp->getName().find("Pawn") != std::string::npos && destination_name.at(1) == '1'){
                        this->promotionAvailable = true;
                    }
                }
                this->board->at(des_row).at(des_col)->setPiece(tmp);
                (*occupiedCells).erase(starting_location_name);
                (*occupiedCells)[destination_name] = tmp->getTeam(); 

            }
        }
        
        void promote(Team *team, Cell *destination){
            std::vector<Chesspiece*> pieces = team->getPieces();
            Chesspiece *p = destination->getPiece();
            int i;
            for (i = 0; i < team->getPieces().size(); ++i){
                if (team->getPieces().at(i)->getName() == p->getName()){
                    break;
                }
            }
            bool announced = false;
            while (true){
                std::string input;
                if (!announced){
                    std::cout << "What do you want to promote your pawn to?\n";
                    std::cout << "1. Queen\n";
                    std::cout << "2. Rook\n";
                    std::cout << "3. Bishop\n";
                    std::cout << "4. Knight\n";
                    std::cin >> input;
                }
                if (input == "1"){
                    delete p;
                    p = new Queen("Promoted " + team->getName() + " Queen", team->getName(), destination->getName());
                    pieces.at(i) = p;
                    destination->setPiece(p);
                    break;
                }
                if (input == "2"){
                    delete p;
                    p = new Rook("Promoted " + team->getName() + " Rook", team->getName(), destination->getName());
                    pieces.at(i) = p;
                    destination->setPiece(p);
                    break;
                }
                if (input == "3"){
                    delete p;
                    p = new Bishop("Promoted " + team->getName() + " Bishop", team->getName(), destination->getName());
                    pieces.at(i) = p;
                    destination->setPiece(p);
                    break;
                }
                if (input == "4"){
                    delete p;
                    p = new Knight("Promoted " + team->getName() + " Knight", team->getName(), destination->getName());
                    pieces.at(i) = p;
                    destination->setPiece(p);
                    break;
                }
            }
        }

        void showAvailableMoves(std::string position){
            Cell *c = getCell(position);
            Chesspiece *p = c->getPiece();
            std::vector<std::string> moves = p->getAvailableMoves();
            for (int i = 0; i < moves.size(); i++){
                std::string tmp_pos = moves.at(i);
                Cell*tmp_cell = getCell(tmp_pos);
                if (tmp_cell->getPiece() != nullptr){
                    if (tmp_cell->getPiece()->getName().find("King") == std::string::npos){
                        tmp_cell->getPiece()->setStatus("threatened");
                    }
                    tmp_cell->getPiece()->changeColor();
                }
                else{
                    tmp_cell->setPrintStatus("*");
                }
            } 
        } 

        void removeAvailableMoves(){
            for (int i = 0; i < 8; ++i){
                for (int j = 0; j < 8; ++j){
                    Cell *c = this->board->at(i).at(j);
                    if (c->getPrintStatus() == "*"){
                        c->setPrintStatus(" ");
                    }
                    if (c->getPiece() != nullptr && c->getPiece()->getStatus() == "threatened"){
                        c->getPiece()->setStatus("active");
                        c->getPiece()->changeColor();
                    }
                }
            }
        }
};

#endif