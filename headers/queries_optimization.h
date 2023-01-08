#ifndef QP
#define QP
#include "../headers/structures.h"
typedef struct Predicates{
    int left_rel;
    int left_col;
    int right_rel;
    int right_col;
}pred;


typedef struct BestTree{
    int relations[4];
    int size;
}besttree;

//interimidiate result for query optimization
typedef struct q_inter{
    int rel_count;
    int relations[4];
    stats stat[4];
}q_inter;

int* produce_next_set(int* set,int n, int size);
int get_number(int* array,int size);
int connected(int j,int* set,pred* join_array,int size_of_array);
besttree* CreateJoinTree(besttree* tree,int j);
void copy_stats(Table* T,int count,int* original_relations, stats** stat);

#endif