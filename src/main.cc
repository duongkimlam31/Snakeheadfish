#include "../include/Chess.h"

int main() {
  system("clear");
  std::cout << "   _____             _        _                    _   ______ "
               "_     _     "
            << std::endl;
  std::cout << "  / ____|           | |      | |                  | | |  "
               "____(_)   | |    "
            << std::endl;
  std::cout << " | (___  _ __   __ _| | _____| |__   ___  __ _  __| | | |__   "
               "_ ___| |__  "
            << std::endl;
  std::cout << "  \\___ \\| '_ \\ / _` | |/ / _ \\ '_ \\ / _ \\/ _` |/ _` | |  "
               "__| | / __| '_ \\ "
            << std::endl;
  std::cout << "  ____) | | | | (_| |   <  __/ | | |  __/ (_| | (_| | | |    | "
               "\\__ \\ | | |"
            << std::endl;
  std::cout << " |_____/|_| |_|\\__,_|_|\\_\\___|_| |_|\\___|\\__,_|\\__,_| "
               "|_|    |_|___/_| |_|"
            << std::endl;
  std::cout << std::endl;
  std::cout << "Press Enter to continue...";
  std::string option;
  getline(std::cin, option);
  system("clear");
  option = "";
  Chessboard *chessboard = new Chessboard();
  Chess chess(chessboard);
  std::cout << "Choose your team:\n";
  std::cout << "1. White\n";
  std::cout << "2. Black\n";
  std::cout << "0. Exit\n";
  while (true) {
    std::cout << "Option: ";
    getline(std::cin, option);
    if (option == "1") {
      option = "white";
      break;
    } else if (option == "2") {
      option = "black";
      break;
    } else if (option == "0") {
      return 0;
    } else {
      std::cout << "Invalid option. Please try again.\n";
      continue;
    }
  }
  system("clear");
  chess.start(option);
  return 0;
}