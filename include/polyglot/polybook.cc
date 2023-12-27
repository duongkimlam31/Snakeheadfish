#ifndef POLYBOOK_H
#define POLYBOOK_H

#include "polykeys.cc"
#include "stdio.h"
#include <cstdint>
#include <fcntl.h>
#include <time.h> 
#include "../../include/chess.hpp"

char RankChar[] = "12345678";
char FileChar[] = "abcdefgh";

typedef struct{
  uint64_t key;
  uint16_t move;
  uint16_t weight;
  uint32_t learn;
} entry;

class PolyBookReader{
  private:
    entry *entries;
    int num_entries;
  public:
    PolyBookReader(char *path){
      FILE *f = fopen(path, "rb"); 
      long size; 
      if (f == NULL){
        fclose(f);
        perror("Can't open opening book\n");
        exit(-1);
      }
      else{
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        if (size < sizeof(entry)){
          perror("No entries found\n");
          exit(-1);
        }
      }
      this->num_entries = size / sizeof(entry);
      this->entries = (entry*) malloc(sizeof(entry)*num_entries);
      rewind(f);
      size_t return_val = fread(entries, sizeof(entry), num_entries, f);
    }

    ~PolyBookReader(){
      free(entries);
    }

    unsigned short endian_swap_u16(unsigned short x) 
    { 
        x = (x>>8) | 
            (x<<8); 
        return x;
    } 

    unsigned int endian_swap_u32(unsigned int x) 
    { 
        x = (x>>24) | 
            ((x<<8) & 0x00FF0000) | 
            ((x>>8) & 0x0000FF00) | 
            (x<<24); 
        return x;
    } 

    uint64_t endian_swap_u64(uint64_t x) 
    { 
        x = (x>>56) | 
            ((x<<40) & 0x00FF000000000000) | 
            ((x<<24) & 0x0000FF0000000000) | 
            ((x<<8)  & 0x000000FF00000000) | 
            ((x>>8)  & 0x00000000FF000000) | 
            ((x>>24) & 0x0000000000FF0000) | 
            ((x>>40) & 0x000000000000FF00) | 
            (x<<56); 
        return x;
    }

    void cleanPolyBook(){

    }

    uint64_t PolyKeyFromBoard(chess::Board chessboard){
      return chessboard.hash();
    }

    chess::Movelist listBookMoves(uint64_t polyKey, chess::Board chessboard){
      int index = 0;
      entry *e;
      uint16_t move;
      chess::Movelist bookMoves;
      int count = 0;
      for (e = this->entries; e < this->entries + this->num_entries; ++e){
        if (polyKey == endian_swap_u64(e->key)){
          move = endian_swap_u16(e->move);
          chess::Move tmpMove = ConvertPolyMoveToSAN(move, chessboard);
          if (tmpMove != chess::Move::NO_MOVE){
            bookMoves.add(tmpMove);
            count++;
          }
        }
      }
      return bookMoves;
    }

    chess::Move ConvertPolyMoveToSAN(uint16_t polyMove, chess::Board chessboard){
      int ff = (polyMove >> 6) & 7;
      int fr = (polyMove >> 9) & 7;
      int tf = (polyMove >> 0) & 7;
      int tr = (polyMove >> 3) & 7;
      int pp = (polyMove >> 12) & 7;
      
      char moveString[6];
      if(pp == 0) {
        sprintf(moveString, "%c%c%c%c",
        FileChar[ff],
        RankChar[fr],
        FileChar[tf],
        RankChar[tr]);
      } else {
        char promChar = 'q';
        switch(pp) {
          case 1: promChar = 'n'; break;
          case 2: promChar = 'b'; break;
          case 3: promChar = 'r'; break;
        }
        sprintf(moveString, "%c%c%c%c%c",
        FileChar[ff],
        RankChar[fr],
        FileChar[tf],
        RankChar[tr],
        promChar);
      }
      chess::Move move = chess::uci::uciToMove(chessboard, moveString);
      chess::Movelist legal_moves;
      chess::movegen::legalmoves(legal_moves, chessboard);
      if (legal_moves.find(move) != -1){
        return move;
      }
      return chess::Move::NO_MOVE;
    }
};

#endif