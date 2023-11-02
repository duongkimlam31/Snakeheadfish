# sudo pip install chess
import chess
import chess.engine
import chess.polyglot
import chess.syzygy
import sys
import os
import pickle
import time


class Chess:
    def __init__(self, state = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"):
        self.state = state
        self.timer = 0
        self.time_limit = 0
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
        # prune endgame legal actions here
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
        ordered_list = []
        for move in new_board.legal_moves:
            if len(str(move)) != 4:
                continue
            destination = chess.parse_square(str(move)[2:])
            if (new_board.is_attacked_by(new_board.turn, destination)):
                ordered_list.insert(0, str(move))
            else:
                ordered_list.append(str(move))
        return ordered_list
    def result(self, state, action):
        new_board = chess.Board(state)
        new_board.push_san(action)
        # handle promotion here
        return new_board.fen()
    def isCutoff(self, state, depth, cutoff_depth):
        new_board = chess.Board(state)
        if new_board.is_checkmate() or new_board.is_stalemate() or new_board.is_insufficient_material() or new_board.can_claim_draw() or new_board.is_fivefold_repetition() or cutoff_depth == depth:
            return True
        return False
    def eval(self, state):
        new_board = chess.Board(state)
        if new_board.is_checkmate():
            if new_board.turn == chess.WHITE:
                return float("-inf")
            else:
                return float("inf")
        elif new_board.is_insufficient_material() or new_board.can_claim_draw() or new_board.is_fivefold_repetition() or new_board.is_stalemate():
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
    def iterative_deepening_minimax(self, time_limit, max_depth):
        self.timer = time.time() + time_limit
        for cutoff_depth in range(1,max_depth+1):
            self.transposition_table = {}
            move = self.minimax(self.state, cutoff_depth)
            if time.time() >= self.timer:
                break
        return move
    def minimax(self, state, cutoff_depth):
        if self.toMove(state) == chess.WHITE:
            value, move = self.max_value_abc(state, float("-inf"), float("inf"), 0, cutoff_depth)
        else:
            value, move = self.min_value_abc(state, float("-inf"), float("inf"), 0, cutoff_depth)
        return move
    def max_value_abc(self, state, alpha, beta, depth, cutoff_depth):
        tmp_state = chess.Board(state)
        zobrist = chess.polyglot.zobrist_hash(tmp_state)
        if zobrist in self.transposition_table:
            return self.transposition_table[zobrist][0], self.transposition_table[zobrist][1]
        if self.isCutoff(state, depth, cutoff_depth):
            return self.eval(state), ""
        val = float("-inf")
        move = ""
        for a in self.actions(state):
            v2, a2 = self.min_value_abc(self.result(state, a), alpha, beta, depth+1, cutoff_depth)
            if v2 > val:
                val, move = v2, a
                alpha = max(val, alpha)
            if val >= beta:
                self.transposition_table[zobrist] = [val, move, depth]
                return val, move
        self.transposition_table[zobrist] = [val, move]
        return val, move
    def min_value_abc(self, state, alpha, beta, depth, cutoff_depth):
        tmp_state = chess.Board(state)
        zobrist = chess.polyglot.zobrist_hash(tmp_state)
        if zobrist in self.transposition_table:
            return self.transposition_table[zobrist][0], self.transposition_table[zobrist][1]
        if self.isCutoff(state, depth, cutoff_depth):
            return self.eval(state), ""
        val = float("inf")
        move = ""
        for a in self.actions(state):
            v2, a2 = self.max_value_abc(self.result(state, a), alpha, beta, depth + 1, cutoff_depth)
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
    # if os.path.isfile("transposition_table.bin") and os.path.getsize("transposition_table.bin") > 0:
    #     hash_file = open("transposition_table.bin", "rb")
    #     game.transposition_table = pickle.load(hash_file)
    move = game.iterative_deepening_minimax(30,5)
    # print(move)
    # move = game.minimax(game.state, 5)
    write_end = int(sys.argv[2])
    pipe = os.fdopen(write_end, "w")

    # Write data to the pipe
    pipe.write(move)
    pipe.flush()  # Flush the buffer to ensure data is sent immediately

    # Close the pipe
    pipe.close()
    # hash_file = open("transposition_table.bin", "wb")
    # pickle.dump(game.transposition_table, hash_file)
    
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

