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
import random

UPPERBOUND = 1
EXACT = 0
LOWERBOUND = -1

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
        
class SnakeheadFish:
    def __init__(self, state = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"):
        self.board = chess.Board(state)
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
            openings.append(entry.move)
        for entry in self.opening_book_reader_2.find_all(board):
            openings.append(entry.move)
        for entry in self.opening_book_reader_3.find_all(board):
            openings.append(entry.move)
        for entry in self.opening_book_reader_4.find_all(board):
            openings.append(entry.move)
        for entry in self.opening_book_reader_5.find_all(board):
            openings.append(entry.move)
        if len(openings) != 0:
            return openings
        ordered_list = []
        for move in board.legal_moves:
            destination = move.to_square
            if (board.is_attacked_by(board.turn, destination)):
                ordered_list.insert(0, move)
            else:
                ordered_list.append(move)
        return ordered_list
    
    def result(self, board, action):
        tmp_board = board.copy()
        tmp_board.push(action)
        return tmp_board
    
    def isCutoff(self, board):
        if board.is_checkmate() or board.is_stalemate() or board.is_insufficient_material() or board.can_claim_draw() or board.is_fivefold_repetition() or board.is_variant_end():
            return True
        return False
    
    def eval(self, board):
        if board.is_checkmate():
            if board.turn == chess.WHITE:
                return float("-inf")
            else:
                return float("inf")
        elif board.is_stalemate() or board.is_insufficient_material() or board.can_claim_draw() or board.is_fivefold_repetition() or board.is_variant_end():
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
    
    def iterative_deepening_minimax(self, board, time_limit, max_depth):
        move = ""
        tmp_board = board.copy()
        for cutoff_depth in range(1,max_depth+1):
            with Timeout(time_limit):
                try:
                    # self.transposition_table = {}
                    move = self.find_move(tmp_board, cutoff_depth)
                except:
                    if move == "":
                        move = self.find_move(tmp_board, cutoff_depth)
                    break
        return move
    
    def quiescent_search(self, board, alpha, beta):
        stand_pat = self.eval(board)
        if stand_pat >= beta:
            return beta
        if alpha < stand_pat:
            alpha = stand_pat
        legal_captures = [move for move in board.legal_moves if board.is_capture(move)]
        for capture in legal_captures:
            score = -self.quiescent_search(self.result(board, capture),-beta, -alpha)
            if score >= beta:
                return beta
            if score > alpha:
                alpha = score
        return alpha
    
    def alpha_beta_negamax(self, board, alpha, beta, depth, turn_multiplier, allow_null_move_prune=True):
        tmp_board = board.copy()
        original_alpha = alpha
        zobrist_key = chess.polyglot.zobrist_hash(tmp_board)
        if zobrist_key in self.transposition_table and self.transposition_table[zobrist_key][1] >= depth:
            if self.transposition_table[zobrist_key][2] == EXACT:
                return self.transposition_table[zobrist_key][0]
            elif self.transposition_table[zobrist_key][2] == LOWERBOUND:
                alpha = max(alpha, self.transposition_table[zobrist_key][0])
            elif self.transposition_table[zobrist_key][2] == UPPERBOUND:
                beta = min(beta, self.transposition_table[zobrist_key][0])
        if alpha >= beta:
            return self.transposition_table[zobrist_key][0]
        if self.isCutoff(board):
            return self.eval(board) * turn_multiplier
        if depth == 0:
            return self.quiescent_search(tmp_board, alpha, beta)
        if allow_null_move_prune and not tmp_board.is_check() and depth >= 3:
            print(4)
            tmp_board.push(chess.Move.null())
            val = -self.alpha_beta_negamax(tmp_board, -beta, -beta + 1, depth - 3, -turn_multiplier, False)
            tmp_board.pop()
            if(val >= beta):
                return val
        val = float("-inf")
        for move in self.actions(board):
            val = max(val, -self.alpha_beta_negamax(self.result(tmp_board, move), -beta, -alpha, depth - 1, -turn_multiplier))
            alpha = max(alpha, val)
            if alpha >= beta:
                break
        self.transposition_table[zobrist_key] = [None, None, None]
        self.transposition_table[zobrist_key][0] = val
        if val <= original_alpha:
            self.transposition_table[zobrist_key][2] = UPPERBOUND
        elif val >= beta:
            self.transposition_table[zobrist_key][2] = LOWERBOUND
        else:
            self.transposition_table[zobrist_key][2] = EXACT
        self.transposition_table[zobrist_key][1] = depth
        return val
    
    def find_move(self, board, depth):
        tmp_board = board.copy()
        if self.toMove(tmp_board) == chess.WHITE:
            turn_multiplier = 1
        else:
            turn_multiplier = -1
        alpha = float('-inf')
        beta = float('inf')
        
        best_evaluation = float("-inf")
        best_move = ""
        for move in self.actions(tmp_board):
            evaluation = -self.alpha_beta_negamax(self.result(tmp_board, move), -beta, -alpha, depth-1, turn_multiplier)
            if evaluation > best_evaluation:
                best_evaluation = evaluation
                best_move = move
            alpha = max(alpha, evaluation)
        return best_move
            
if __name__ == "__main__":    
    ai = SnakeheadFish(sys.argv[1])
    if os.path.isfile("transposition_table.bin") and os.path.getsize("transposition_table.bin") > 0:
        hash_file = open("transposition_table.bin", "rb")
        ai.transposition_table = pickle.load(hash_file)
    begin = time.time()
    move = ai.iterative_deepening_minimax(ai.board, 100, 5)
    end = time.time()
    print("Total run time:", abs(begin-end), "seconds")
    # move = ai.minimax(ai.state, 5)
    write_end = int(sys.argv[2])
    pipe = os.fdopen(write_end, "w")

    # Write data to the pipe
    pipe.write(str(move))
    pipe.flush()  # Flush the buffer to ensure data is sent immediately

    # Close the pipe
    pipe.close()

    hash_file = open("transposition_table.bin", "wb")
    pickle.dump(ai.transposition_table, hash_file)
    
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