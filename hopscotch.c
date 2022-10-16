#include "structures.h"
#include <math.h>

int hash2(int key){
    return key;
}

int get_hash_i(int key,int size_of_array){
    return hash2(key)%size_of_array;
}

tuple* create_array(int n){
    return malloc(n*sizeof(tuple));
}

int insert(tuple* array, tuple element, int H){
    
}