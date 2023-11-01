# sudo pip install chess
import chess
import chess.engine
import chess.polyglot
import chess.syzygy
import sys
import os
import time


class Chess:
    def __init__(self, state = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", cutoff_depth = 3):
        self.board = chess.Board(state)
        self.cutoff_depth = cutoff_depth
        self.transposition_table = {}
        self.opening_book_reader_1 = chess.polyglot.open_reader("book_openings/standard_opening_book.bin")
        self.opening_book_reader_2 = chess.polyglot.open_reader("book_openings/titans_opening_book.bin")
        self.opening_book_reader_3 = chess.polyglot.open_reader("book_openings/komodo.bin")
        self.opening_book_reader_4 = chess.polyglot.open_reader("book_openings/gm2001.bin")
        self.opening_book_reader_5 = chess.polyglot.open_reader("book_openings/rodent.bin")
    def toMove(self, state):
        new_board = chess.Board(state)
        return new_board.turn
    def actions(self, state):
        new_board = chess.Board(state)
        openings = []
        for entry in self.opening_book_reader_1.find_all(new_board):
            openings.append(str(entry.move))
        for entry in self.opening_book_reader_2.find_all(new_board):
            openings.append(str(entry.move))
        for entry in self.opening_book_reader_3.find_all(new_board):
            openings.append(str(entry.move))
        for entry in self.opening_book_reader_4.find_all(new_board):
            openings.append(str(entry.move))
        for entry in self.opening_book_reader_5.find_all(new_board):
            openings.append(str(entry.move))
        if len(openings) != 0:
            return openings
        return [str(move) for move in new_board.legal_moves]
    def result(self, state, action):
        new_board = chess.Board(state)
        new_board.push_san(action)
        return new_board.fen()
    def isCutoff(self, state, depth):
        new_board = chess.Board(state)
        if new_board.is_checkmate() or new_board.is_stalemate() or new_board.is_insufficient_material() or new_board.can_claim_draw() or new_board.is_fivefold_repetition() or self.cutoff_depth == depth:
            return True
        return False
    def eval(self, state):
        new_board = chess.Board(state)
        if new_board.is_checkmate() or new_board.is_stalemate():
            if new_board.turn == chess.WHITE:
                return float("-inf")
            else:
                return float("inf")
        elif new_board.is_insufficient_material() or new_board.can_claim_draw() or new_board.is_fivefold_repetition():
          return 0
        pieces = {}
        for square in chess.SQUARES:
            piece = new_board.piece_at(square)
            if piece != None:
                if piece.symbol() not in pieces:
                    pieces[piece.symbol()] = 1
                else:
                    pieces[piece.symbol()] += 1
        eval_score = 0
        for piece in pieces:
            if piece == "P":
                eval_score += 1 * pieces[piece]
            elif piece == "p":
                eval_score += -1 * pieces[piece]
            elif piece == "R":
                eval_score += 5 * pieces[piece]
            elif piece == "r":
                eval_score += -5 * pieces[piece]
            elif piece == "N" or piece == "B":
                eval_score += 3 * pieces[piece]
            elif piece == "n" or piece == "b":
                eval_score += -3 * pieces[piece]
            elif piece == "Q":
                eval_score += 9 * pieces[piece]
            elif piece == "q":
                eval_score += -9 * pieces[piece]
        return eval_score
    def minimax(self, state):
        if self.toMove(state) == chess.WHITE:
            value, move = self.max_value_abc(state, float("-inf"), float("inf"), 0)
        else:
            value, move = self.min_value_abc(state, float("-inf"), float("inf"), 0)
        return move
    def max_value_abc(self, state, alpha, beta, depth):
        tmp_state = chess.Board(state)
        zobrist = chess.polyglot.zobrist_hash(tmp_state)
        if zobrist in self.transposition_table:
            return self.transposition_table[zobrist][0], self.transposition_table[zobrist][1]
        if self.isCutoff(state, depth):
            return self.eval(state), ""
        val = float("-inf")
        move = ""
        for a in self.actions(state):
            v2, a2 = self.min_value_abc(self.result(state, a), alpha, beta, depth+1)
            if v2 > val:
                val, move = v2, a
                alpha = max(val, alpha)
            if val >= beta:
                self.transposition_table[zobrist] = [val, move]
                return val, move
        self.transposition_table[zobrist] = [val, move]
        return val, move
    def min_value_abc(self, state, alpha, beta, depth):
        tmp_state = chess.Board(state)
        zobrist = chess.polyglot.zobrist_hash(tmp_state)
        if zobrist in self.transposition_table:
            return self.transposition_table[zobrist][0], self.transposition_table[zobrist][1]
        if self.isCutoff(state, depth):
            return self.eval(state), ""
        val = float("inf")
        move = ""
        for a in self.actions(state):
            v2, a2 = self.max_value_abc(self.result(state, a), alpha, beta, depth + 1)
            if v2 < val:
                val, move = v2, a
                beta = min(val, beta)
            if val <= alpha:
                self.transposition_table[zobrist] = [val, move]
                return val, move
        self.transposition_table[zobrist] = [val, move]
        return val, move
        
            
if __name__ == "__main__":
    game = Chess(sys.argv[1])
    board = game.board
    move = game.minimax(board.fen())
    write_end = int(sys.argv[2])
    print(write_end)
    pipe = os.fdopen(write_end, "w")

    # Write data to the pipe
    pipe.write(move)
    pipe.flush()  # Flush the buffer to ensure data is sent immediately

    # Close the pipe
    pipe.close()
    # # Create a new chess board

    # # board = chess.Board()
    # # board2 = board.copy()
    # board = chess.Board(sys.argv[1])

    # # Print the initial chessboard
    # # print(board.fen)

    # # List all legal moves from the current position
    # # legal_moves = list(board.legal_moves)
    # # print("Legal moves:", legal_moves)

    # # Make a move 
    # # board.push_san("e2e4")
    # # print(board.fen)

    # # Book openings 
    # # return NULL if empty


    # # print(board)

    # # Syzygy table for the endgame 
    # # return NULL if empty
    # # Loop through each legal move and compare dtz value
    # # tablebase =chess.syzygy.open_tablebase("./3-4-5_pieces_Syzygy")
    # # print(tablebase.get_dtz(board))

    # # White team is a max player => trying to get the evaluation to be as positive as possible
    # # Black team is a min player => trying to get the evaluation to be as negative as possible


    # # Get the location of the pieces
    # # for square in chess.SQUARES:
    # #     piece = board.piece_at(square)
    # #     if piece != None:
    # #         print(chess.square_name(square), ": ", piece)


    # # engine = chess.engine.SimpleEngine.popen_uci("../stockfish-windows-x86-64-avx2/stockfish/stockfish-windows-x86-64-avx2.exe")
    # # # evaluation = engine.analyse(board, chess.engine.Limit(time=0.15, depth=3))
    # # decision = engine.play(board, chess.engine.Limit(time=3, depth=22))
    # # # print(evaluation)
    # # print(decision.move)
    # # engine.quit()

    # # Zobrist hashing
    # # chess.polyglot.zobrist_hash(board)


