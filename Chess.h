#include <vector>
#include <string>
#include "Team.h"
#include "Chesspiece.h"
#include "Chessboard.h"
#include "Pawn.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "King.h"
#include "Queen.h"
#include "Cell.h"
#include <cctype>


#ifndef GAME_H 
#define GAME_H
class Chess{
    private:
        Chessboard *chessboard;
        Team *white;
        Team *black;
    public:
        Chess(Team *white, Team *black, Chessboard *chessboard){
            this->white = white;
            this->black = black;
            this->chessboard = chessboard;
        }
        ~Chess(){};
        void setup(){
            Pawn *wp1 = new Pawn("White Pawn 1", "white", "A2");
            Pawn *wp2 = new Pawn("White Pawn 2", "white", "B2");
            Pawn *wp3 = new Pawn("White Pawn 3", "white", "C2");
            Pawn *wp4 = new Pawn("White Pawn 4", "white", "D2");
            Pawn *wp5 = new Pawn("White Pawn 5", "white", "E2");
            Pawn *wp6 = new Pawn("White Pawn 6", "white", "F2");
            Pawn *wp7 = new Pawn("White Pawn 7", "white", "G2");
            Pawn *wp8 = new Pawn("White Pawn 8", "white", "H2");

            Pawn *bp1 = new Pawn("Black Pawn 1", "black", "A7");
            Pawn *bp2 = new Pawn("Black Pawn 2", "black", "B7");
            Pawn *bp3 = new Pawn("Black Pawn 3", "black", "C7");
            Pawn *bp4 = new Pawn("Black Pawn 4", "black", "D7");
            Pawn *bp5 = new Pawn("Black Pawn 5", "black", "E7");
            Pawn *bp6 = new Pawn("Black Pawn 6", "black", "F7");
            Pawn *bp7 = new Pawn("Black Pawn 7", "black", "G7");
            Pawn *bp8 = new Pawn("Black Pawn 8", "black", "H7");

            Rook *wr1 = new Rook("White Rook 1", "white", "A1");
            Rook *wr2 = new Rook("White Rook 2", "white", "H1");
            Rook *br1 = new Rook("Black Rook 1", "black", "A8");
            Rook *br2 = new Rook("Black Rook 2", "black", "H8");

            Knight *wk1 = new Knight("White Knight 1", "white", "B1");
            Knight *wk2 = new Knight("White Knight 2", "white", "G1");
            Knight *bk1 = new Knight("Black Knight 1", "black", "B8");
            Knight *bk2 = new Knight("Black Knight 2", "black", "G8");

            Bishop *wb1 = new Bishop("White Bishop 1", "white", "C1");
            Bishop *wb2 = new Bishop("White Bishop 2", "white", "F1");
            Bishop *bb1 = new Bishop("Black Bishop 1", "black", "C8");
            Bishop *bb2 = new Bishop("Black Bishop 2", "black", "F8");

            King *wk = new King("White King", "white", "E1");
            King *bk = new King("Black King", "black", "E8");

            Queen *wq = new Queen("White Queen", "white", "D1");
            Queen *bq = new Queen("Black Queen", "black", "D8");

            std::vector<Chesspiece*> row1 = {wr1, wk1, wb1, wq, wk, wb2, wk2, wr2};
            std::vector<Chesspiece*> row2 = {wp1, wp2, wp3, wp4, wp5, wp6, wp7, wp8};
            std::vector<Chesspiece*> row7 = {bp1, bp2, bp3, bp4, bp5, bp6, bp7, bp8};
            std::vector<Chesspiece*> row8 = {br1, bk1, bb1, bq, bk, bb2, bk2, br2};
            white->setKing(wk);
            black->setKing(bk);

            for (int i = 0; i < 8; ++i){
                this->black->addPiece(row7.at(i));
                this->black->addPiece(row8.at(i));
                this->chessboard->getBoard()->at(0).at(i)->setPiece(row8.at(i));
                this->chessboard->getBoard()->at(1).at(i)->setPiece(row7.at(i));
                this->white->addPiece(row2.at(i));
                this->white->addPiece(row1.at(i));
                this->chessboard->getBoard()->at(6).at(i)->setPiece(row2.at(i));
                this->chessboard->getBoard()->at(7).at(i)->setPiece(row1.at(i));
            }
            this->chessboard->findOccupiedCells();
        }

        bool inCheck(int turn, Chessboard *board, Chesspiece *king){
            if (turn % 2 == 0){
                std::set<std::string> *black_team_moves = board->getBlackTeamAvailableMoves();
                return black_team_moves->find(king->getPosition()) != black_team_moves->end();
            }
            else{
                std::set<std::string> *white_team_moves = board->getWhiteTeamAvailableMoves();
                return white_team_moves->find(king->getPosition()) != white_team_moves->end();
            }
        }

        bool checkmate(Chessboard *currentState, int turn){
            if (turn % 2 == 0){
                std::vector<Chesspiece*> whiteTeam = this->white->getPieces();
                for (int i = 0; i < whiteTeam.size(); ++i){
                    std::string starting_position = whiteTeam.at(i)->getPosition();
                    if (whiteTeam.at(i)->getStatus() == "active"){
                        std::vector<std::string> moves = whiteTeam.at(i)->getAvailableMoves();
                        for (int j = 0; j < moves.size(); ++j){
                            Chessboard tmp_state = *currentState;
                            std::string destination = moves.at(j);
                            Cell start_cell = *tmp_state.getCell(starting_position);
                            Cell desination_cell = *tmp_state.getCell(destination);
                            tmp_state.generateAllMoves();
                            tmp_state.movePiece(&start_cell, &desination_cell);
                            if (!inCheck(turn, &tmp_state, this->white->getKing())){
                                return false;
                            }
                        }
                    }
                }
            }
            else if (turn % 2 != 0){
                std::vector<Chesspiece*> blackTeam = this->black->getPieces();
                for (int i = 0; i < blackTeam.size(); ++i){
                    std::string starting_position = blackTeam.at(i)->getPosition();
                    if (blackTeam.at(i)->getStatus() == "active"){
                        std::vector<std::string> moves = blackTeam.at(i)->getAvailableMoves();
                        for (int j = 0; j < moves.size(); ++j){
                            Chessboard tmp_state = *currentState;
                            std::string destination = moves.at(j);
                            Cell start_cell = *tmp_state.getCell(starting_position);
                            Cell desination_cell = *tmp_state.getCell(destination);
                            tmp_state.generateAllMoves();
                            tmp_state.movePiece(&start_cell, &desination_cell);
                            if (!inCheck(turn, &tmp_state, this->black->getKing())){
                                return false;
                            }
                        }
                    }
                }
            }
            return true;
        }

        void start(){
            std::string input;
            int turn = 0;
            bool announced = false;
            while (this->white->getKing()->getStatus() != "captured" && this->black->getKing()->getStatus() != "captured") {
                this->chessboard->generateAllMoves();
                if (!announced){
                    this->chessboard->printBoard();
                }
                if (turn % 2 == 0){
                    // if (inCheck(turn, this->chessboard, white->getKing())){
                    //     if (checkmate(this->chessboard, turn)){
                    //         std::cout << "Checkmate\n" << std::endl;
                    //     }
                    //     else{
                    //         std::cout << "White team is in check.\n" << std::endl;
                    //     }
                    // }
                    if (!announced){
                        std::cout << "White team's turn: \n";
                        announced = true;
                    }
                }
                else if (turn % 2 != 0){
                    // if (inCheck(turn, this->chessboard, black->getKing())){
                    //     if (checkmate(this->chessboard, turn)){
                    //         std::cout << "Checkmate\n" << std::endl;
                    //         break;
                    //     }
                    //     else{
                    //         std::cout << "Black team is in check.\n" << std::endl;
                    //     }
                    // }
                    if (!announced){
                        std::cout << "Black team's turn: \n";
                        announced = true;
                    }
                }
                std::cin >> input;
                if (input == "0"){
                    break;
                }
                if (input.size() != 2){
                    std::cout << "Invalid Input. Please try again. \n";
                    continue;
                }
                char col_name = input.at(0);
                if (!isalpha(col_name)){
                    std::cout << "Invalid Input. Please try again. \n";
                    continue;
                }
                col_name = tolower(col_name);
                if (col_name < 'a' or col_name > 'h'){
                    std::cout << "Invalid Input. Please try again. \n";
                    continue;
                }
                char row_name = input.at(1);
                if (!isdigit(row_name) or row_name < '1' or row_name > '8'){
                    std::cout << "Invalid Input. Please try again. \n";
                    continue;
                }
                Cell *c1 = this->chessboard->getCell(input);
                if(this->chessboard->getCell(input)->getPiece() == nullptr){
                    std::cout << "This cell is empty. Please try a different cell. \n";
                    continue;
                }
                if (turn % 2 == 0 && c1->getPiece()->getTeam() != "white"){
                    std::cout << "This piece does not belong to your team. Please choose a different one. \n";
                    continue;
                }
                if (turn % 2 != 0 && c1->getPiece()->getTeam() != "black"){
                    std::cout << "This piece does not belong to your team. Please choose a different one. \n";
                    continue;
                }
                this->chessboard->showAvailableMoves(c1->getName());
                this->chessboard->printBoard();
                if (c1->getPiece()->getAvailableMoves().size() == 0){
                    std::cout << "This piece doesn't have any available move. Please try again.\n";
                    continue;
                }
                std::string input_move;
                bool nextTurn = false;
                bool announced_2 = false;
                while (true){  
                    if (!announced_2){
                        std::cout << "Press 1 to go back." << std::endl;
                        announced_2 = true;
                    }
                    std::cin >> input_move;
                    if (input_move == "1"){
                        this->chessboard->removeAvailableMoves();
                        announced = false;
                        break;
                    }
                    bool canMove = false;
                    if (input_move.size() != 2){
                        std::cout << "Invalid Input. Please try again. \n";
                        continue;
                    }
                    char col_name_move = input_move.at(0);
                    if (!isalpha(col_name_move)){
                        std::cout << "Invalid Input. Please try again. \n";
                        continue;
                    }
                    col_name_move = tolower(col_name_move);
                    if (col_name_move < 'a' or col_name_move > 'h'){
                        std::cout << "Invalid Input. Please try again. \n";
                        continue;
                    }
                    char row_name_move = input_move.at(1);
                    if (!isdigit(row_name_move) or row_name_move < '1' or row_name_move > '8'){
                        std::cout << "Invalid Input. Please try again. \n";
                        continue;
                    } 
                    std::vector<std::string> legalMoves = c1->getPiece()->getAvailableMoves();
                    for(int i = 0; i < legalMoves.size(); ++i){
                        if (tolower(legalMoves.at(i).at(0)) == tolower(input_move.at(0)) && legalMoves.at(i).at(1) == input_move.at(1)){
                            canMove = true;
                            break;
                        }
                    }
                    if (canMove){
                        nextTurn = true;
                        break;
                    }
                    else{
                        std::cout << "Can't move to that cell. Please try again. \n";
                    }
                }
                if (nextTurn){
                    Cell *c2 = this->chessboard->getCell(input_move);
                    this->chessboard->movePiece(c1, c2);
                    if (this->chessboard->getPromotionAvailable()){
                        this->chessboard->printBoard();
                        if (turn %2 == 0){
                            this->chessboard->promote(this->white, c2);
                        }
                        else{
                            this->chessboard->promote(this->black, c2);
                        }
                        this->chessboard->setPromotionAvailable(false);
                    }
                    this->chessboard->removeAvailableMoves();
                    announced = false;
                    ++turn;
                }
            }
            this->chessboard->printBoard();
            if (this->white->getKing()->getStatus() == "captured"){
                std::cout << "Black team won.\n";
            }
            else if (this->black->getKing()->getStatus() == "captured"){
                std::cout << "White team won.\n";
            }
        }
};



#endif