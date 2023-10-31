# sudo pip install chess
import chess
import chess.polyglot
import chess.syzygy
import sys

# Create a new chess board

# board = chess.Board()
# board2 = board.copy()
board = chess.Board(sys.argv[1])
# print(sys.argv[1])

# Print the initial chessboard
# print(board.fen)

# List all legal moves from the current position
# legal_moves = list(board.legal_moves)
# print("Legal moves:", legal_moves)

# Make a move 
# board.push_san("e2e4")
# print(board.fen)

# Book openings 
# return NULL if empty

i = 1
openings = set()
reader1 = chess.polyglot.open_reader("book_openings/standard_opening_book.bin")
reader2 = chess.polyglot.open_reader("book_openings/titans_opening_book.bin")
reader3 =chess.polyglot.open_reader("book_openings/komodo.bin")
reader4 = chess.polyglot.open_reader("book_openings/gm2001.bin")
reader5 = chess.polyglot.open_reader("book_openings/rodent.bin")
# while i != 0: 
for entry in reader1.find_all(board):
    openings.add(str(entry.move))
for entry in reader2.find_all(board):
    openings.add(str(entry.move))
for entry in reader3.find_all(board):
    openings.add(str(entry.move))
for entry in reader4.find_all(board):
    openings.add(str(entry.move))
for entry in reader5.find_all(board):
    openings.add(str(entry.move))
# print(openings)

# print(board)


# Syzygy table for the endgame 
# return NULL if empty
# Loop through each legal move and compare dtz value
# tablebase =chess.syzygy.open_tablebase("./3-4-5_pieces_Syzygy")
# print(tablebase.get_dtz(board))

# White team is a max player => trying to get the evaluation to be as positive as possible
# Black team is a min player => trying to get the evaluation to be as negative as possible


