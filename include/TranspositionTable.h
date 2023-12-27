#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "chess.hpp"
#include <list>
#include <stdint.h>
#include <iostream>

// Transposition table entry
typedef struct {
  float best_value;
  chess::Move best_move;
  int depth;
  int flag;
} tt_entry_t;

typedef struct node{
  uint64_t zobrist_key;
  tt_entry_t entry;
  struct node *next_node = NULL;
} node_t;

typedef struct {
  node_t *list = NULL;
} linked_list_t;

class TranspositionTable{
  private:
    const static int size = (32 * 0x100000)/sizeof(node_t); //Allocate 48 MB for the table
    linked_list_t *table;
  public:
    TranspositionTable(){
      table = new linked_list_t[size];
    }
    ~TranspositionTable(){
      for (unsigned int i = 0; i < size; ++i){
        linked_list_t *linked_list = &table[i];
        while (linked_list->list != NULL){
          node_t *tmp_node = linked_list->list;
          linked_list->list = linked_list->list->next_node;
          free(tmp_node);

        }
      }
      delete[] table;
    }
    int get_size(){
      return size;
    }
    int hash_function(uint64_t key){
      return key % size;
    }

    void update_table(uint64_t key, const tt_entry_t &value){
      int hash_val = hash_function(key);
      linked_list_t *linked_list = &table[hash_val];
      node_t *front_node = linked_list->list;
      node_t *front_front_node = linked_list->list;
      if (front_node == NULL){
        node_t *tmp_node = (node_t *) malloc(sizeof(node_t));
        tmp_node->entry = value;
        tmp_node->next_node = NULL; 
        tmp_node->zobrist_key = key;
        linked_list->list = tmp_node;
        return;
      }

      while (front_node != NULL){
        if (front_node->zobrist_key == key){
          front_node->entry = value;
          return;
        }
        front_front_node = front_node;
        front_node = front_node->next_node;
      }
      node_t *tmp_node = (node_t *) malloc(sizeof(node_t));
      tmp_node->entry = value;
      tmp_node->next_node = NULL; 
      tmp_node->zobrist_key = key;
      front_front_node->next_node = tmp_node;
    }

    bool is_valid_key(uint64_t key){
      int hash_val = hash_function(key);
      bool key_exists = false;
      linked_list_t *linked_list = &table[hash_val];
      node_t *front_node = linked_list->list;
      while (front_node != NULL){
        if (front_node->zobrist_key == key){
          key_exists = true;
          break;
        }
        front_node = front_node->next_node;
      }
      return key_exists;
    }

    tt_entry_t probe_table(uint64_t key){
      int hash_val = hash_function(key);
      bool key_exists = false;
      linked_list_t *linked_list = &table[hash_val];
      node_t *front_node = linked_list->list;
      tt_entry_t entry;
      while (front_node != NULL){
        if (front_node->zobrist_key == key){
          key_exists = true;
          entry = front_node->entry;
          break;
        }
        front_node = front_node->next_node;
      }
      if (!key_exists){
        std::cout << "Key doesn't exist\n";
        exit(-1);
      }
      return entry;
    }
};

#endif