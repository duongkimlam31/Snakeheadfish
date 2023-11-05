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
        self.nodes_traversed = 0
        
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
        return list(board.legal_moves)
    
    def mvv_lva_sort(self, board, move):
        mvv = {chess.PAWN: 1, chess.KNIGHT: 3, chess.BISHOP: 3, chess.ROOK: 5,chess.QUEEN: 9, chess.KING: 0}
        lva = {chess.PAWN: 9, chess.KNIGHT: 5, chess.BISHOP: 5, chess.ROOK: 3,chess.QUEEN: 1, chess.KING: 0}
        if board.is_capture(move):
            victim_square = board.piece_at(move.to_square)
            aggressor_square = board.piece_at(move.from_square)
            if victim_square != None:
                victim = victim_square.piece_type
                aggressor = aggressor_square.piece_type
                return mvv[victim] + lva[aggressor]
            else:
                return 1
        return 0
    
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
        piece_value = {"P": 1, "p": -1, "N": 3, "n": -3 ,"B": 3, "b": -3 ,"R": 5, "r": -5, "Q": 9, "q": -9, "K": 0, "k": 0}
        eval_score = 0
        for piece in pieces:
            eval_score += piece_value[piece] * pieces[piece]
        if self.toMove(board) == chess.WHITE:
            return eval_score
        else:
            return -eval_score
    
    def iterative_deepening_minimax(self, board, time_limit, max_depth):
        move = ""
        tmp_board = board.copy()
        for cutoff_depth in range(1,max_depth+1):
            print(cutoff_depth)
            with Timeout(time_limit):
                try:
                    move = self.find_move(tmp_board, cutoff_depth)
                except:
                    if move == "":
                        move = self.find_move(tmp_board, cutoff_depth)
                    break
        return move
    
    def quiescence_search(self, board, alpha, beta):
        self.nodes_traversed += 1
        stand_pat = self.eval(board)
        if stand_pat >= beta:
            return beta
        if alpha < stand_pat:
            alpha = stand_pat
        legal_captures = [move for move in board.legal_moves if board.is_capture(move)]
        legal_captures.sort(key=lambda move: self.mvv_lva_sort(board, move))
        legal_captures.reverse()
        for capture in legal_captures:
            score = -self.quiescence_search(self.result(board, capture),-beta, -alpha)
            if score >= beta:
                return beta
            if score > alpha:
                alpha = score
        return alpha

    def alpha_beta_negamax(self, board, alpha, beta, depth, allow_null_move_prune=True):
        self.nodes_traversed += 1
        tmp_board = board.copy()
        if self.isCutoff(tmp_board):
            return self.eval(tmp_board)
        if depth == 0:
            return self.quiescence_search(tmp_board, alpha, beta)
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
        if allow_null_move_prune and not tmp_board.is_check() and depth >= 3 and not (beta-alpha > 1):
            tmp_board.push(chess.Move.null())
            val = -self.alpha_beta_negamax(tmp_board, -beta, -beta + 1, depth - 3, False)
            tmp_board.pop()
            if(val >= beta):
                return val
        val = float("-inf")
        legal_moves = self.actions(tmp_board)
        legal_moves.sort(key=lambda move: self.mvv_lva_sort(tmp_board, move))
        legal_moves.reverse()
        moves_searched = 0
        for move in legal_moves:
            if moves_searched == 0:
                val = max(val, -self.alpha_beta_negamax(self.result(tmp_board, move), -beta, -alpha, depth - 1))
            else:
                if moves_searched >= 4 and depth >= 3:
                    if not tmp_board.is_check() and not (beta-alpha > 1) and not tmp_board.is_capture(move) and not move.promotion:
                        val = -self.alpha_beta_negamax(self.result(tmp_board, move), -(alpha + 1), -alpha, depth - 2)
                else:
                    val = alpha + 1
                if val > alpha:
                    val = -self.alpha_beta_negamax(self.result(tmp_board, move), -(alpha+1), -alpha, depth - 1)
                    if val > alpha and val < beta:
                        val = -self.alpha_beta_negamax(self.result(tmp_board, move), -beta, -alpha, depth - 1)
            alpha = max(alpha, val)
            if alpha >= beta:
                break
            moves_searched += 1
        self.transposition_table[zobrist_key] = [None, None, None]
        if val <= original_alpha:
            self.transposition_table[zobrist_key][2] = UPPERBOUND
        elif val >= beta:
            self.transposition_table[zobrist_key][2] = LOWERBOUND
        else:
            self.transposition_table[zobrist_key][2] = EXACT
        self.transposition_table[zobrist_key][1] = depth
        self.transposition_table[zobrist_key][0] = val
        return val
    
    def find_move(self, board, depth):
        best_evaluated_move = None
        best_value = float('-inf')
        alpha = float('-inf')
        beta = float('inf')
        for move in board.legal_moves:
            boardValue = -self.alpha_beta_negamax(self.result(board, move), -beta, -alpha, depth - 1)
            if boardValue > best_value:
                best_value = boardValue
                best_evaluated_move = move
            alpha = max(alpha, boardValue)
        return best_evaluated_move
            
if __name__ == "__main__":    
    ai = SnakeheadFish(sys.argv[1])
    if os.path.isfile("transposition_table.bin") and os.path.getsize("transposition_table.bin") > 0:
        hash_file = open("transposition_table.bin", "rb")
        ai.transposition_table = pickle.load(hash_file)
    begin = time.time()
    move = ai.iterative_deepening_minimax(ai.board, 20, 6)
    end = time.time()
    print("Total run time:", abs(begin-end), "seconds")
    print("Total traversed nodes:", ai.nodes_traversed)
    write_end = int(sys.argv[2])
    pipe = os.fdopen(write_end, "w")

    # Write data to the pipe
    pipe.write(str(move))
    pipe.flush()  # Flush the buffer to ensure data is sent immediately

    # Close the pipe
    pipe.close()

    hash_file = open("transposition_table.bin", "wb")
    pickle.dump(ai.transposition_table, hash_file)

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