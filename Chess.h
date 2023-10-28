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
    public:
        Chess(Chessboard *chessboard){
            this->chessboard = chessboard;
            this->chessboard->setup();
        }
        
        ~Chess(){
            delete this->chessboard;
        };

        bool inCheck(int turn, Chessboard *board, Chesspiece *king){
            bool check = false;
            if (turn % 2 == 0){
                std::set<std::string> *black_team_moves = board->getBlackTeamAvailableMoves();
                auto it = black_team_moves->find(king->getPosition());
                check = (it != black_team_moves->end());
            }
            else{
                std::set<std::string> *white_team_moves = board->getWhiteTeamAvailableMoves();
                auto it = white_team_moves->find(king->getPosition());
                check = (it != white_team_moves->end());
            }
            return check;
        }

        bool inCheckForSimulation(int turn, Chessboard *board){
            bool check = false;
            std::string current_team = "";
            std::set<std::string> enemy_team_moves; 
            if (turn % 2 == 0){
                current_team = "white";
                enemy_team_moves = *board->getBlackTeamAvailableMoves();
            }
            else{
                current_team = "black";
                enemy_team_moves = *board->getWhiteTeamAvailableMoves();
            }
            Chesspiece *piece;
            bool stop_loop = false;
            for (int i = 0; i < 8; ++i){
                if (stop_loop){
                    break;
                }
                for (int j = 0; j < 8; ++j){
                    piece = board->getBoard()->at(i).at(j)->getPiece();
                    if (piece != nullptr && piece->getTeam() == current_team && piece->getName().find("King") != std::string::npos){
                        stop_loop = true;
                        break;
                    }
                }
            }
            auto it = enemy_team_moves.find(piece->getPosition());
            check = (it != enemy_team_moves.end());
            // std::cout << check << std::endl;
            return check;
        }

        void removeEnPassant(int turn){
            Team *team_to_remove;
            if (turn % 2 == 0 && this->chessboard->getEnPassantAvailability() == "black"){
                team_to_remove = this->chessboard->getBlackTeam();
            }
            else if (turn % 2 != 0 && this->chessboard->getEnPassantAvailability() == "white"){
                team_to_remove = this->chessboard->getWhiteTeam();
            }
            else{
                return;
            }
            std::vector<Chesspiece*> pieces = team_to_remove->getPieces();
            for (int i = 0; i < pieces.size(); ++i){
                if (pieces.at(i)->getName().find("Pawn") != std::string::npos){
                    Pawn *p = dynamic_cast<Pawn*>(pieces.at(i));
                    p->setEnPassant("");
                }
            }
        }

        void pruneAvailableMoves(std::set<std::string> legalMoves, int turn){
            Team *team;
            if (turn % 2 == 0){
                team = this->chessboard->getWhiteTeam();
            }
            else{
                team = this->chessboard->getBlackTeam();
            }
            std::vector<Chesspiece*> team_pieces = team->getPieces();
            for (int i = 0; i < team_pieces.size(); ++i){
                if (team_pieces.at(i)->getStatus() != "captured"){
                    std::vector<std::string> moves = team_pieces.at(i)->getAvailableMoves();
                    std::vector<std::string> prunedMoves;
                    for (int j = 0; j < moves.size(); ++j){
                        auto it = legalMoves.find(moves.at(j));
                        if (it != legalMoves.end()){
                            prunedMoves.push_back(moves.at(j));
                        }
                    }
                    team_pieces.at(i)->setAvailableMoves(prunedMoves);
                }
            }
        }

        void addPoints(int points, int turn){
            Team *current_team;
            if (turn % 2 == 0){
                current_team = this->chessboard->getWhiteTeam();
            }
            else{
                current_team = this->chessboard->getBlackTeam();
            }
            current_team->addPoints(points);
        }

        std::vector<std::string> legalMoveCheck(std::string starting_location, Chessboard *board, int turn){
            std::vector<std::string> legalMoves;
            bool legal = false;
            Cell c1 = *board->getCell(starting_location);
            Chessboard next_state;
            std::vector<std::string> moves = c1.getPiece()->getAvailableMoves();
            for (int i = 0; i < moves.size(); ++i){
                next_state = *board;
                c1 = *next_state.getCell(starting_location);
                Cell c2 = *next_state.getCell(moves.at(i));
                next_state.movePiece(&c1, &c2);
                next_state.printBoard();
                if (turn % 2 == 0){
                    next_state.generateBlackTeamMoves();
                }
                else{
                    next_state.generateWhiteTeamMoves();
                }
                if (!inCheckForSimulation(turn, &next_state)){
                    legalMoves.push_back(c2.getName());
                }
            }
            return legalMoves;
        }

        std::set<std::string> allLegalMoves(int turn){
            std::set<std::string> all_legal_moves;
            std::vector<Chesspiece*> current_team;
            if (turn % 2 == 0){
                current_team = this->chessboard->getWhiteTeam()->getPieces();
            }
            else{
                current_team = this->chessboard->getBlackTeam()->getPieces();
            }
            for (int i = 0; i < current_team.size(); ++i){
                std::string position = current_team.at(i)->getPosition();
                if (current_team.at(i)->getStatus() != "captured"){
                    std::vector<std::string> tmp_legal_moves = legalMoveCheck(position, this->chessboard, turn);
                    if (tmp_legal_moves.size() > 0){
                        all_legal_moves.insert(current_team.at(i)->getPosition());
                        for (int j = 0; j < tmp_legal_moves.size(); ++j){
                            all_legal_moves.insert(tmp_legal_moves.at(j));
                        }
                    }
                }
            }
            return all_legal_moves;
        }
        
        void start(){
            std::string input;
            int turn = 0;
            bool announced = false;
            bool checkmate = false;
            bool stalemate = false;
            while (true) {
                this->chessboard->generateAllMoves();
                std::set<std::string> all_legal_moves;
                all_legal_moves = allLegalMoves(turn);
                pruneAvailableMoves(all_legal_moves, turn);
                if (!announced){
                    if (turn % 2 == 0){
                        if (inCheck(turn, this->chessboard, this->chessboard->getWhiteTeam()->getKing())){
                            if (all_legal_moves.size() == 0){
                                checkmate = true;
                                break;
                            }
                            else{
                                this->chessboard->getWhiteTeam()->getKing()->setStatus("checked");
                                this->chessboard->getWhiteTeam()->getKing()->changeColor();
                                std::cout << "White team is in check.\n" << std::endl;
                                this->chessboard->showLegalMoves(all_legal_moves, turn);
                            }
                        }
                        else {
                            this->chessboard->getBlackTeam()->getKing()->setStatus("active");
                            this->chessboard->getBlackTeam()->getKing()->changeColor();
                        }
                    }
                    else if (turn % 2 != 0){
                        if (inCheck(turn, this->chessboard, this->chessboard->getBlackTeam()->getKing())){
                            if (all_legal_moves.size() == 0){
                                checkmate = true;
                                break;
                            }
                            else{
                                this->chessboard->getBlackTeam()->getKing()->setStatus("checked");
                                this->chessboard->getBlackTeam()->getKing()->changeColor();
                                std::cout << "Black team is in check.\n" << std::endl;
                                this->chessboard->showLegalMoves(all_legal_moves, turn);
                            }
                        }
                        else {
                            this->chessboard->getWhiteTeam()->getKing()->setStatus("active");
                            this->chessboard->getWhiteTeam()->getKing()->changeColor();
                        }
                        if (all_legal_moves.size() == 0){
                            stalemate = true;
                            break;
                        }
                    }
                    this->chessboard->printBoard();
                }
                if (turn % 2 != 0 && !announced){
                    std::cout << "Black team's turn: \n";
                    announced = true;
                }
                else if (turn % 2 == 0 && !announced){
                    std::cout << "White team's turn: \n";
                    announced = true;
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
                col_name = toupper(col_name);
                if (col_name < 'A' or col_name > 'H'){
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
                if (this->chessboard->getWhiteTeam()->getKing()->getStatus() != "checked" && this->chessboard->getBlackTeam()->getKing()->getStatus() != "checked"){
                    this->chessboard->showAvailableMoves(c1->getName());
                }
                this->chessboard->printBoard();
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
                    col_name_move = toupper(col_name_move);
                    if (col_name_move < 'A' or col_name_move > 'H'){
                        std::cout << "Invalid Input. Please try again. \n";
                        continue;
                    }
                    char row_name_move = input_move.at(1);
                    if (!isdigit(row_name_move) or row_name_move < '1' or row_name_move > '8'){
                        std::cout << "Invalid Input. Please try again. \n";
                        continue;
                    } 
                    std::vector<std::string> legalMoveChecks = c1->getPiece()->getAvailableMoves();
                    for(int i = 0; i < legalMoveChecks.size(); ++i){
                        if (toupper(legalMoveChecks.at(i).at(0)) == toupper(input_move.at(0)) && legalMoveChecks.at(i).at(1) == input_move.at(1)){
                            canMove = true;
                            break;
                        }
                    }
                    if (canMove){
                        nextTurn = true;
                        break;
                    }
                    else{
                        std::cout << "That move is not legal. Please try again. \n";
                    }
                }
                if (nextTurn){
                    this->chessboard->removeAvailableMoves();
                    Cell *c2 = this->chessboard->getCell(input_move);
                    int points = this->chessboard->movePiece(c1, c2);
                    addPoints(points, turn);
                    if (this->chessboard->getPromotionAvailable()){
                        this->chessboard->printBoard();
                        if (turn %2 == 0){
                            this->chessboard->promote(this->chessboard->getWhiteTeam(), c2);
                        }
                        else{
                            this->chessboard->promote(this->chessboard->getBlackTeam(), c2);
                        }
                        this->chessboard->setPromotionAvailable(false);
                    }
                    announced = false;
                    ++turn;
                    if (this->chessboard->getWhiteTeam()->getKing()->getStatus() == "checked"){
                        this->chessboard->getWhiteTeam()->getKing()->setStatus("active");
                        this->chessboard->getWhiteTeam()->getKing()->changeColor();
                    }
                    else if (this->chessboard->getBlackTeam()->getKing()->getStatus() == "checked"){
                        this->chessboard->getBlackTeam()->getKing()->setStatus("active");
                        this->chessboard->getBlackTeam()->getKing()->changeColor();
                    }
                    removeEnPassant(turn);
                }
            }
            this->chessboard->printBoard();
            if (stalemate){
                std::cout << "Stalemate" << std::endl;
            }
            else if (turn % 2 != 0 && checkmate){
                std::cout << "Checkmate" << std::endl;
                std::cout << "The white team has won" << std::endl;
            }
            else if (turn % 2 == 0 && checkmate){
                std::cout << "Checkmate" << std::endl;
                std::cout << "The black team has won" << std::endl;
            }
            std::cout << "White team's total points: " << this->chessboard->getWhiteTeam()->getPoints() << std::endl;
            std::cout << "Black team's total points: " << this->chessboard->getBlackTeam()->getPoints() << std::endl;
        }
};



#endif