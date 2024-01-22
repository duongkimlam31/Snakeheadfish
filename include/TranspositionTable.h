#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "chess.hpp"
#include <list>
#include <stdint.h>
#include <iostream>

// Transposition table entry
typedef struct {
  int best_value;
  chess::Move best_move;
  int depth;
  int flag;
  bool extension;
} tt_entry_t;

typedef struct node{
  uint64_t zobrist_key;
  tt_entry_t *entry = NULL;
} node_t;


class TranspositionTable{
  private:
    const static int size = (0x100000*32); //Allocate 32 MB for the table
    node_t *table;
  public:
    TranspositionTable(){
      table = new node_t[size];
    }
    ~TranspositionTable(){
      for (int i = 0; i < size; ++i){
        free(table[i].entry);
      }
      delete[] table;
    }
    int get_size(){
      return size;
    }
    int hash_function(uint64_t key){
      return key % size;
    }

    void update_table(uint64_t key, tt_entry_t value){
      int hash_val = hash_function(key);
      table[hash_val].zobrist_key = key;
      if (table[hash_val].entry == NULL){
        tt_entry_t *tmp_entry = (tt_entry_t *) malloc(sizeof(tt_entry_t));
        tmp_entry->best_value = value.best_value;
        tmp_entry->depth = value.depth;
        tmp_entry->extension = value.extension;
        tmp_entry->flag = value.flag;
        tmp_entry->best_move = value.best_move;
        table[hash_val].entry = tmp_entry;
      }
      else{
        table[hash_val].entry->best_value = value.best_value;
        table[hash_val].entry->depth = value.depth;
        table[hash_val].entry->extension = value.extension;
        table[hash_val].entry->flag = value.flag;
        table[hash_val].entry->best_move = value.best_move;
      }
    }

    bool is_valid_key(uint64_t key){
      int hash_val = hash_function(key);
      node_t node = table[hash_val];
      return (node.entry != NULL && node.zobrist_key == key);
    }

    tt_entry_t probe_table(uint64_t key){
      int hash_val = hash_function(key);
      return *table[hash_val].entry;
    }
};

#endif