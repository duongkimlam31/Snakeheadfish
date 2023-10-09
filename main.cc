#include "Chess.h"
using namespace std;

int main() {
    Team *white = new Team("white");
    Team *black = new Team("black");
    Chessboard *chessboard = new Chessboard();
    Chess chess(white, black, chessboard);
    chess.setup();
    chess.start();
    return 0;
}