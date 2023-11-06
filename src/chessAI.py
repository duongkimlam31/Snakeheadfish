# sudo pip install chess
import chess.engine
import chess.polyglot
import chess.syzygy
import sys
import os
import pickle
import time
import signal
import piece_square_tables

# Global variables
UPPERBOUND = 1
EXACT = 0
LOWERBOUND = -1

ASPIRATION_WINDOW_CUT = 50


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
    def __init__(self, search_depth = 6, search_time = 10, state = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"):
        self.transposition_table = {}
        self.killer_moves_table = {}
        self.history_table = {chess.WHITE: [[0 for _ in range(64)] for _ in range(64)], chess.BLACK : [[0 for _ in range(64)] for _ in range(64)]}
        self.opening_book_reader = chess.polyglot.MemoryMappedReader("book_openings/titans_opening_book.bin")
        self.syzygy_tablebase =chess.syzygy.open_tablebase("./3-4-5_pieces_Syzygy")
        self.nodes_traversed = 0
        self.search_depth = search_depth
        self.search_time = search_time
        self.middle_game_table, self.end_game_table = piece_square_tables.init_table()
        self.gamephaseInc = [0,0,1,1,1,1,2,2,4,4,0,0]
        self.futility_margin = [0, 100, 300, 500]
        for depth in range(self.search_depth):
            self.killer_moves_table[depth] = [None for _ in range(2)]
        
    def toMove(self, board):
        return board.turn
    
    def actions(self, board):
        legal_moves = list(board.legal_moves)
        return legal_moves
        
    def get_ordered_moves(self, board):
        legal_captures = [move for move in self.actions(board) if board.is_capture(move)]
        legal_non_captures = [move for move in self.actions(board) if not board.is_capture(move)]
        legal_captures.sort(key=lambda move: self.mvv_lva_heuristic(board, move))
        legal_non_captures.sort(key=lambda move: self.history_heuristic(move, self.toMove(board)))
        legal_captures.reverse()
        legal_non_captures.reverse()
        return legal_captures, legal_non_captures
    
    def history_heuristic(self, move, turn):
        return self.history_table[turn][move.from_square][move.to_square]
    
    def mvv_lva_heuristic(self, board, move):
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
        dtz = self.syzygy_tablebase.get_dtz(board)
        if dtz != None:
            return dtz
        mg = {}
        eg = {}
        gamePhase = 0
        mg[chess.WHITE] = 0
        mg[chess.BLACK] = 0
        eg[chess.WHITE] = 0
        eg[chess.BLACK] = 0
        pieces_dict = [chess.PAWN, chess.KNIGHT, chess.BISHOP, chess.ROOK, chess.QUEEN, chess.KING]
        for square in chess.SQUARES:
            piece = board.piece_at(square)
            if piece != None:
                if piece.color == chess.WHITE:
                    pc = pieces_dict.index(piece.piece_type) * 2
                else:
                    pc = pieces_dict.index(piece.piece_type) * 2 + 1
                mg[piece.color] += self.middle_game_table[pc][square]
                eg[piece.color] += self.end_game_table[pc][square]
                gamePhase += self.gamephaseInc[pc]
        mgScore = mg[self.toMove(board)] - mg[not self.toMove(board)]
        egScore = eg[self.toMove(board)] - eg[not self.toMove(board)]
        mgPhase = gamePhase
        if mgPhase > 24:
            mgPhase = 24
        egPhase = 24 - mgPhase
        val_2 = (mgScore * mgPhase + egScore * egPhase) / 24
        return val_2
    
    def iterative_deepening_search(self, board):
        opening_move = None
        try:
            opening_move = self.opening_book_reader.weighted_choice(board).move
            return opening_move
        except:
            move = None
            best_value = float('-inf')
            alpha = float('-inf')
            beta = float('inf')
            legal_captures, legal_non_captures = self.get_ordered_moves(board.copy())
            legal_moves = legal_captures + legal_non_captures
            cutoff_depth = 1
            with Timeout(self.search_time):
                try:
                    while cutoff_depth <= self.search_depth:
                        for a in legal_moves:
                            val = -self.alpha_beta_negamax(self.result(board.copy(), a), -beta, -alpha, cutoff_depth - 1, 1)
                            if val > best_value:
                                best_value = val
                                move = a
                        # Aspiration Window 
                        if best_value <= alpha or best_value >= beta:
                            alpha = float("-inf")
                            beta = float("inf")
                            continue
                        alpha = best_value - ASPIRATION_WINDOW_CUT
                        beta = best_value + ASPIRATION_WINDOW_CUT
                        if val == float("inf") and move != None:
                            break
                        cutoff_depth += 1
                except:
                    if not move:
                        for a in legal_moves:
                            val = -self.alpha_beta_negamax(self.result(board.copy(), a), -beta, -alpha, cutoff_depth - 1, 1)
                            if val > best_value:
                                best_value = val
                                move = a
                            alpha = max(alpha, best_value)
            return move
    
    def quiescence_search(self, board, alpha, beta):
        self.nodes_traversed += 1
        stand_pat = self.eval(board)
        if stand_pat >= beta:
            return beta
        if alpha < stand_pat:
            alpha = stand_pat
        legal_captures = [move for move in board.legal_moves if board.is_capture(move)]
        legal_captures.sort(key=lambda move: self.mvv_lva_heuristic(board, move))
        legal_captures.reverse()
        for capture in legal_captures:
            score = -self.quiescence_search(self.result(board, capture),-beta, -alpha)
            if score >= beta:
                return beta
            if score > alpha:
                alpha = score
        return alpha

    def alpha_beta_negamax(self, board, alpha, beta, depth, ply, allow_null_move_prune=True):
        self.nodes_traversed += 1
        tmp_board = board.copy()
        if self.isCutoff(tmp_board):
            return self.eval(tmp_board)
        if depth == 0:
            return self.quiescence_search(tmp_board, alpha, beta)
        original_alpha = alpha
        zobrist_key = chess.polyglot.zobrist_hash(tmp_board)
        tt_move = None
        if zobrist_key in self.transposition_table and self.transposition_table[zobrist_key][1] >= depth:
            tt_move = self.transposition_table[zobrist_key][3]
            if self.transposition_table[zobrist_key][2] == EXACT:
                return self.transposition_table[zobrist_key][0]
            elif self.transposition_table[zobrist_key][2] == LOWERBOUND:
                alpha = max(alpha, self.transposition_table[zobrist_key][0])
            elif self.transposition_table[zobrist_key][2] == UPPERBOUND:
                beta = min(beta, self.transposition_table[zobrist_key][0])
            if alpha >= beta:
                return self.transposition_table[zobrist_key][0]
        static_eval = self.eval(tmp_board)
        # If not a pv-node and not in check
        allow_futility_prune = False
        if not (beta-alpha > 1) and not tmp_board.is_check():
            # null-move pruning
            if allow_null_move_prune and depth >= 3:
                tmp_board.push(chess.Move.null())
                val = -self.alpha_beta_negamax(tmp_board, -beta, -beta + 1, depth - 3, ply + 3, False)
                tmp_board.pop()
                if(val >= beta):
                    return val
            # Razoring
            razor_score = static_eval + 1
            q_score = self.quiescence_search(tmp_board, alpha, beta)
            if razor_score < beta:
                if depth == 1:
                    if q_score > razor_score:
                        return q_score
                    else:
                        return razor_score
            razor_score += 1
            if razor_score < beta and depth < 4:
                if q_score < beta:
                    if q_score > razor_score:
                        return q_score
                    else:
                        return razor_score
            if depth < 4 and alpha != float("inf") and alpha != float("-inf") and beta != float("inf") and beta != float("-inf") and (static_eval + self.futility_margin[depth]) <= alpha:
                allow_futility_prune = True
        # Internal iterative deepening
        if depth >= 5 and (beta-alpha > 1):
            depth -= 1
        val = float("-inf")
        # Sort move using mvv-lva and history heuristic
        legal_captures, legal_non_captures = self.get_ordered_moves(tmp_board)
        
        killer_moves = []
        for i in range(len(self.killer_moves_table[ply])):
            killer_move = self.killer_moves_table[ply][i]
            for j in range(len(legal_captures)):
                if killer_move == legal_captures[j]:
                    killer_moves.append(killer_move)
            for k in range(len(legal_non_captures)):
                if killer_move == legal_non_captures[k]:
                    killer_moves.append(killer_move)
        
        # Move order: transposition entry -> legal captures -> killer moves -> legal non captures
        if tt_move != None:
            legal_moves = [tt_move]
        else:
            legal_moves = []
        legal_moves += legal_captures + killer_moves + legal_non_captures
        
        moves_searched = 0
        best_move = None
        for move in legal_moves:
            best_move = move
            # Futility pruning
            if depth == 1 and allow_futility_prune and moves_searched != 0 and not tmp_board.is_capture(move) and not move.promotion:
                tmp_board.push(move)
                if not tmp_board.is_check():
                    tmp_board.pop()
                    try:
                        previous_move = tmp_board.pop()
                        if not tmp_board.is_capture(previous_move):
                            tmp_board.push(previous_move)
                            continue
                    except:
                        pass
            if moves_searched == 0:
                val = max(val, -self.alpha_beta_negamax(self.result(tmp_board, move), -beta, -alpha, depth - 1, ply + 1))
            # Late move reductions
            else:
                if moves_searched >= 4 and depth >= 3:
                    if not tmp_board.is_check() and not (beta-alpha > 1) and not tmp_board.is_capture(move) and not move.promotion:
                        val = -self.alpha_beta_negamax(self.result(tmp_board, move), -(alpha + 1), -alpha, depth - 2, ply + 2)
                else:
                    val = alpha + 1
                # PV-search
                if val > alpha:
                    val = -self.alpha_beta_negamax(self.result(tmp_board, move), -(alpha+1), -alpha, depth - 1, ply + 1)
                    if val > alpha and val < beta:
                        val = -self.alpha_beta_negamax(self.result(tmp_board, move), -beta, -alpha, depth - 1, ply + 1)
            alpha = max(alpha, val)
            if alpha >= beta:
                break
            moves_searched += 1
            
        # Add zobrist key to the transposition table
        self.transposition_table[zobrist_key] = [None, None, None, None]
        if val <= original_alpha:
            self.transposition_table[zobrist_key][2] = UPPERBOUND
        elif val >= beta:
            self.transposition_table[zobrist_key][2] = LOWERBOUND
        else:
            self.transposition_table[zobrist_key][2] = EXACT
        self.transposition_table[zobrist_key][1] = depth
        self.transposition_table[zobrist_key][0] = val
        self.transposition_table[zobrist_key][3] = best_move
        
        # Add value to the history table
        self.history_table[self.toMove(tmp_board)][best_move.from_square][best_move.to_square] += depth**2
        
        # add killer moves to the killer moves table if applicable
        if best_move != self.killer_moves_table[ply][0] and not tmp_board.is_capture(best_move):
            for i in range(len(self.killer_moves_table[ply])-3, -1, -1):
                self.killer_moves_table[ply][i+1] = self.killer_moves_table[ply][i]
            self.killer_moves_table[ply][0] = best_move
        return val
    
    def find_move(self, board):
        best_evaluated_move = None
        best_value = float('-inf')
        alpha = float('-inf')
        beta = float('inf')
        legal_captures, legal_non_captures = self.get_ordered_moves(board.copy())
        legal_moves = legal_captures + legal_non_captures
        for move in legal_moves:
            boardValue = -self.alpha_beta_negamax(self.result(board, move), -beta, -alpha, self.search_depth - 1, 1)
            if boardValue > best_value:
                best_value = boardValue
                best_evaluated_move = move
            alpha = max(alpha, boardValue)
        return best_evaluated_move
            
if __name__ == "__main__":    
    chessboard = chess.Board(sys.argv[1])
    ai = SnakeheadFish(6, 100)
    if os.path.isfile("tables/transposition_table.bin") and os.path.getsize("tables/transposition_table.bin") > 0:
        transposition_table_file = open("tables/transposition_table.bin", "rb")
        ai.transposition_table = pickle.load(transposition_table_file)
        
    if os.path.isfile("tables/history_table.bin") and os.path.getsize("tables/history_table.bin") > 0:
        history_table_file = open("tables/history_table.bin", "rb")
        ai.history_table = pickle.load(history_table_file)
    
    if os.path.isfile("tables/game_state_table.bin") and os.path.getsize("tables/game_state_table.bin") > 0:
        game_state_table_file = open("tables/game_state_table.bin", "rb")
        game_state_table = pickle.load(game_state_table_file)
        ai.middle_game_table, ai.end_game_table = game_state_table[0], game_state_table[1]
    else:
        game_state_table = [ai.middle_game_table, ai.end_game_table]
        
    begin = time.time()
    move = ai.iterative_deepening_search(chessboard)
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
    
    transposition_table_file = open("tables/transposition_table.bin", "wb")
    pickle.dump(ai.transposition_table, transposition_table_file)
    
    history_table_file = open("tables/history_table.bin", "wb")
    pickle.dump(ai.history_table, history_table_file)
    
    game_state_table_file = open("tables/game_state_table.bin", "wb")
    pickle.dump(game_state_table, game_state_table_file)

    # # engine = chess.engine.SimpleEngine.popen_uci("../stockfish-windows-x86-64-avx2/stockfish/stockfish-windows-x86-64-avx2.exe")
    # # # evaluation = engine.analyse(board, chess.engine.Limit(time=0.15, depth=3))
    # # decision = engine.play(board, chess.engine.Limit(time=3, depth=22))
    # # # print(evaluation)
    # # print(decision.move)
    # # engine.quit()

    # # Zobrist hashing
    # # chess.polyglot.zobrist_hash(board)