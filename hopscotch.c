#include "structures.h"
#include <math.h>
#include <stdlib.h>


// node of hopscotch array
typedef struct node {
    struct tuple info; //the tuple
    char  bitmap[64];   //the bitmap
    
} node;

int hash2(int key){
    return key;
}

int get_hash_i(int key,int size_of_array){
    return hash2(key)%size_of_array;
}

node* create_array(int n){
    
    return malloc(n*sizeof(node));
}

int insert(node* array, tuple element, int H){
    array[0].info=element;
    array[0].bitmap[0]=1;
    return 1;
}

int main(){
    node* array=create_array(3);

    tuple t;
    t.key=0;
    t.payload=0;
    insert(array,t,2);
}