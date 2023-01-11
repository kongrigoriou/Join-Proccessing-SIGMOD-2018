#ifndef HOPSCOTCH
#define HOPSCOTCH

#include <stdint.h>
#include "../headers/structures.h"
#include "../headers/list.h"
#include "../headers/bitmap.h"

// node of hopscotch array
typedef struct node {
    struct tuple info;  //the tuple
    bitmap_t  bitmap;   //the bitmap
    int occupied;        //whether node is empty or not
    //List* duplicate;
    List* overflow;
} node;

typedef struct hopscotch_array {
    node* array;
    uint64_t H;
    uint64_t size;
} hop;

typedef struct result_node {
    int index;  //the tuple
    
    int occupied;       //whether node is empty or not
} result_node;

uint64_t hash2(uint64_t key);
int hash(int key,int size_of_array);
node* create_array_of_hop(int n, int h);
hop* create_array(int n, int i);
void destroy_array(node* array, int size);
void destroy_hop(hop* hops);
int get_H(hop* n);   
int bitmapisfull(bitmap_t bitmap, int size);
int get_first_1(bitmap_t bitmap, int size);
int closest_empty_spot(node* array,int start, int size);
void print_array(node* array, int size);
int resize(hop** hops_, tuple element, int n, int h);
int dist(int j, int hash,int n);
int insert(hop* array, tuple element);
List* search(hop* array,tuple element);

#endif
