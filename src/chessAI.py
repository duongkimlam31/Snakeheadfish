# sudo pip install chess
import chess
import chess.engine
import chess.polyglot
import chess.syzygy
import sys
import os
import pickle
import time
import signal

class TimeoutError(Exception):
    """
    Custom error for Timeout class.
    """

    pass


class Timeout:
    """
    A timeout handler with context manager.
    Based on UNIX signals.
    """

    def __init__(self, seconds=1, error_message="Timeout"):
        self.seconds = seconds
        self.error_message = error_message

    def handle_timeout(self, signum, frame):
        raise TimeoutError(self.error_message)

    def __enter__(self):
        signal.signal(signal.SIGALRM, self.handle_timeout)
        signal.alarm(self.seconds)

    def __exit__(self, type, value, traceback):
        signal.alarm(0)
        
class Chess:
    def __init__(self, state = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"):
        self.board = chess.Board(state)
        self.opening_phase = True
        self.nodes = 0
        self.pruned_nodes = 0
        self.transposition_table = {}
        self.opening_book_reader_1 = chess.polyglot.open_reader("book_openings/standard_opening_book.bin")
        self.opening_book_reader_2 = chess.polyglot.open_reader("book_openings/titans_opening_book.bin")
        self.opening_book_reader_3 = chess.polyglot.open_reader("book_openings/komodo.bin")
        self.opening_book_reader_4 = chess.polyglot.open_reader("book_openings/gm2001.bin")
        self.opening_book_reader_5 = chess.polyglot.open_reader("book_openings/rodent.bin")
    def toMove(self, board):
        return board.turn
    def actions(self, board):
        # prune endgame legal actions here
        openings = []
        for entry in self.opening_book_reader_1.find_all(board):
            openings.append(str(entry.move))
        for entry in self.opening_book_reader_2.find_all(board):
            openings.append(str(entry.move))
        for entry in self.opening_book_reader_3.find_all(board):
            openings.append(str(entry.move))
        for entry in self.opening_book_reader_4.find_all(board):
            openings.append(str(entry.move))
        for entry in self.opening_book_reader_5.find_all(board):
            openings.append(str(entry.move))
        if len(openings) != 0:
            return openings
        ordered_list = []
        for move in board.legal_moves:
            if len(str(move)) != 4:
                continue
            destination = chess.parse_square(str(move)[2:])
            if (board.is_attacked_by(board.turn, destination)):
                ordered_list.insert(0, str(move))
            else:
                ordered_list.append(str(move))
        return ordered_list
    def result(self, board, action):
        tmp_board = board.copy()
        tmp_board.push_san(action)
        # handle promotion here
        return tmp_board
    def isCutoff(self, board, depth, cutoff_depth):
        if board.is_checkmate() or board.is_stalemate() or board.is_insufficient_material() or board.can_claim_draw() or board.is_fivefold_repetition() or cutoff_depth == depth:
            return True
        return False
    def eval(self, board):
        if board.is_checkmate():
            if board.turn == chess.WHITE:
                return float("-inf")
            else:
                return float("inf")
        elif board.is_insufficient_material() or board.can_claim_draw() or board.is_fivefold_repetition() or board.is_stalemate():
          return 0
        pieces = {}
        for square in chess.SQUARES:
            piece = board.piece_at(square)
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
        move = ""
        for cutoff_depth in range(1,max_depth+1):
            with Timeout(time_limit):
                try:
                    self.transposition_table = {}
                    move = self.minimax(self.board, cutoff_depth)
                except:
                    if move == "":
                        move = self.minimax(self.board, cutoff_depth)
                    break
        return move
    def minimax(self, board, cutoff_depth):
        if self.toMove(board) == chess.WHITE:
            value, move = self.max_value_abc(board, float("-inf"), float("inf"), 0, cutoff_depth)
        else:
            value, move = self.min_value_abc(board, float("-inf"), float("inf"), 0, cutoff_depth)
        return move
    def max_value_abc(self, board, alpha, beta, depth, cutoff_depth, allow_null = True):
        self.nodes += 1
        tmp_state = board.copy()
        zobrist = chess.polyglot.zobrist_hash(tmp_state)
        if zobrist in self.transposition_table:
            return self.transposition_table[zobrist][0], self.transposition_table[zobrist][1]
        if self.isCutoff(tmp_state, depth, cutoff_depth):
            return self.eval(tmp_state), ""
        if depth + 3 <= cutoff_depth and not tmp_state.is_check() and allow_null and self.has_non_pawn_material(tmp_state):
            tmp_state.push(chess.Move.null())
            val, move = -self.min_value_abc(tmp_state, alpha, beta, depth + 3, cutoff_depth, False)
            tmp_state.pop()
            if val >= beta:
                self.pruned_nodes += 1
                self.transposition_table[zobrist] = [val, move]
                return val, move
        val = float("-inf")
        move = ""
        for a in self.actions(tmp_state):
            v2, a2 = self.min_value_abc(self.result(tmp_state, a), alpha, beta, depth+1, cutoff_depth)
            if v2 > val:
                val, move = v2, a
                alpha = max(val, alpha)
            if val >= beta:
                self.transposition_table[zobrist] = [val, move]
                self.pruned_nodes += 1
                return val, move
        self.transposition_table[zobrist] = [val, move]
        return val, move
    def min_value_abc(self, board, alpha, beta, depth, cutoff_depth, allow_null = True):
        self.nodes += 1
        tmp_state = board.copy()
        zobrist = chess.polyglot.zobrist_hash(tmp_state)
        if zobrist in self.transposition_table:
            return self.transposition_table[zobrist][0], self.transposition_table[zobrist][1]
        if self.isCutoff(tmp_state, depth, cutoff_depth):
            return self.eval(tmp_state), ""
        if depth + 3 <= cutoff_depth and not tmp_state.is_check() and allow_null and self.has_non_pawn_material(tmp_state):
            tmp_state.push(chess.Move.null())
            val, move = -self.max_value_abc(tmp_state, alpha, beta, depth + 3, cutoff_depth, False)
            tmp_state.pop()
            if val <= alpha:
                self.pruned_nodes += 1
                self.transposition_table[zobrist] = [val, move]
                return val, move
        val = float("inf")
        move = ""
        for a in self.actions(tmp_state):
            v2, a2 = self.max_value_abc(self.result(tmp_state, a), alpha, beta, depth + 1, cutoff_depth)
            if v2 < val:
                val, move = v2, a
                beta = min(val, beta)
            if val <= alpha:
                self.pruned_nodes += 1
                self.transposition_table[zobrist] = [val, move]
                return val, move
        self.transposition_table[zobrist] = [val, move]
        return val, move
    
    def has_non_pawn_material(self, board):
        non_pawn_material = {
            chess.KNIGHT: 3,
            chess.BISHOP: 3,
            chess.ROOK: 5,
            chess.QUEEN: 9
        }
        total_non_pawn_material = 0
        for square, piece in board.piece_map().items():
            if piece.piece_type in non_pawn_material:
                total_non_pawn_material += non_pawn_material[piece.piece_type]
        return total_non_pawn_material >= 10
    
if __name__ == "__main__":
    game = Chess(sys.argv[1])
    # if os.path.isfile("transposition_table.bin") and os.path.getsize("transposition_table.bin") > 0:
    #     hash_file = open("transposition_table.bin", "rb")
    #     game.transposition_table = pickle.load(hash_file)
    begin = time.time()
    move = game.iterative_deepening_minimax(200, 4)
    end = time.time()
    print("Total run time:", abs(begin-end), "seconds")
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

