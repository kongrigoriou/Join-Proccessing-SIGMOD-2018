#ifndef HOPSCOTCH
#define HOPSCOTCH

#include <stdint.h>
#include "structures.h"

// node of hopscotch array
typedef struct node {
    struct tuple info;  //the tuple
    uint32_t  bitmap;   //the bitmap
    int occupied;        //whether node is empty or not
} node;

typedef struct result_node {
    int index;  //the tuple
    
    int occupied;       //whether node is empty or not
} result_node;

int insert(node* new_array,tuple element,int size);
uint32_t hash2(uint32_t key);
int hash(int key,int size_of_array);
node* create_array(int n);
int get_H(int n);
int bitmapisfull(uint32_t bitmap, int size);
int get_first_1(uint32_t bitmap, int size);
int closest_empty_spot(node* array,int start, int size);
void print_array(node* array, int size);
int resize(node** array, tuple element, int n);
int dist(int j, int hash,int n);
int insert(node* array, tuple element, int n);
result_node* search(node* array,tuple element, int size);

#endif