#include "Chess.h"

int main() {
    Chessboard *chessboard = new Chessboard();
    Chess chess(chessboard);
    std::string option = "";
    std::cout << "Choose your team:\n";
    std::cout << "0. White\n";
    std::cout << "1. Black\n";
    while (true){
        std::cout << "Option: ";
        std::cin >> option;
        if (option == "0"){
            option = "white";
            break;
        } 
        else if (option == "1"){
            option = "black";
            break;
        }
        else{
            std::cout << "Invalid option. Please try again.\n";
            continue;
        }
    }
    chess.start(option);
    return 0;
}