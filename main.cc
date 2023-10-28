#include "Chess.h"
using namespace std;

int main() {
    Chessboard *chessboard = new Chessboard();
    Chess chess(chessboard);
    chess.start();
    return 0;
}