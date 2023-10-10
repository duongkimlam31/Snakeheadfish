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
            return check;
        }

        bool checkmateCheck(int turn, Chessboard *board, Team *team){
            std::vector<Chesspiece*> pieces = team->getPieces();
            Chessboard current_state;
            for (int i = 0; i < pieces.size(); ++i){
                Chesspiece *tmp_piece = pieces.at(i);
                if (tmp_piece->getStatus() != "captured"){
                    std::string current_position = tmp_piece->getPosition();
                    std::vector<std::string> available_moves = tmp_piece->getAvailableMoves();
                    for (int j = 0; j < available_moves.size(); ++j){
                        current_state = *board;
                        std::string destination = available_moves.at(j);
                        Cell c1 = *current_state.getCell(current_position);
                        Cell c2 = *current_state.getCell(destination);
                        current_state.movePiece(&c1, &c2);
                        if (turn % 2 == 0){
                            current_state.generateBlackTeamMoves();
                        }
                        else{
                            current_state.generateWhiteTeamMoves();
                        }
                        if (!inCheckForSimulation(turn, &current_state)){
                            return false;
                        }
                    }
                }
            }
            return true;
        }

        bool legalMoveCheck2(std::string starting_location, std::string destination, Chessboard *board, int turn){
            Chessboard next_state;
            next_state = *board;
            Cell c1 = *next_state.getCell(starting_location);
            Cell c2 = *next_state.getCell(destination);
            next_state.movePiece(&c1, &c2);
            if (turn % 2 == 0){
                next_state.generateBlackTeamMoves();
            }
            else{
                next_state.generateWhiteTeamMoves();
            }
            bool legal = !inCheckForSimulation(turn, &next_state);
            return legal;
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
                next_state = *board;
                Cell c2 = *next_state.getCell(moves.at(i));
                next_state.movePiece(&c1, &c2);
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
                current_team = this->white->getPieces();
            }
            else{
                current_team = this->black->getPieces();
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
                if (!announced){
                    if (turn % 2 == 0){
                        if (inCheck(turn, this->chessboard, white->getKing())){
                            if (checkmateCheck(turn, this->chessboard, this->white)){
                                checkmate = true;
                                break;
                            }
                            else{
                                white->getKing()->setStatus("checked");
                                white->getKing()->changeColor();
                                std::cout << "White team is in check.\n" << std::endl;
                                this->chessboard->showLegalMoves(all_legal_moves, turn);
                            }
                        }
                        else {
                            black->getKing()->setStatus("active");
                            black->getKing()->changeColor();
                        }
                    }
                    else if (turn % 2 != 0){
                        if (inCheck(turn, this->chessboard, black->getKing())){
                            if (checkmateCheck(turn, this->chessboard, this->black)){
                                checkmate = true;
                                break;
                            }
                            else{
                                black->getKing()->setStatus("checked");
                                black->getKing()->changeColor();
                                std::cout << "Black team is in check.\n" << std::endl;
                                this->chessboard->showLegalMoves(all_legal_moves, turn);
                            }
                        }
                        else {
                            white->getKing()->setStatus("active");
                            white->getKing()->changeColor();
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
                if (c1->getPiece()->getAvailableMoves().size() == 0){
                    std::cout << "This piece doesn't have any available move. Please try again.\n";
                    continue;
                }
                if (legalMoveCheck(input, this->chessboard, turn).size() == 0){
                    std::cout << "The King is in check. Please choose a different move.\n";
                    continue;
                }
                if (white->getKing()->getStatus() != "checked" && black->getKing()->getStatus() != "checked"){
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
                    if (!legalMoveCheck2(input, input_move, this->chessboard, turn)){
                        std::cout << "Invalid Input. Please try again. \n";
                        continue;
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
                    this->chessboard->removeAvailableMoves();
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
                    announced = false;
                    ++turn;
                    if (this->white->getKing()->getStatus() == "checked"){
                        this->white->getKing()->setStatus("active");
                        this->white->getKing()->changeColor();
                    }
                    else if (this->black->getKing()->getStatus() == "checked"){
                        this->black->getKing()->setStatus("active");
                        this->black->getKing()->changeColor();
                    }
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
        }
};



#endif