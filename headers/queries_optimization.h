#ifndef QP
#define QP
#include "../headers/structures.h"
#include "../headers/queries.h"
#include <math.h>
typedef struct Predicates{
    int left_rel;
    int left_col;
    int right_rel;
    int right_col;
}pred;


typedef struct BestTree{
    int relations[4];
    int size;
    char query[50];
}besttree;

//interimidiate result for query optimization
typedef struct q_inter{
    int rel_count;
    int relations[4];
    int columns[4];
    stats* stat[4];
}q_inter;

typedef struct join_op{
    int left;
    int right;
    int left_col;
    int right_col;
}join_op;

int* produce_next_set(int* set,int n, int size);
int get_number(int* array,int size);
int connected(int j,int set[4],pred* join_array,int size_of_array);
besttree* CreateJoinTree(besttree* tree,int j);
void copy_stats(Table* T,int count,int* original_relations, stats** stat);
int update_filter(Table* T,int rel, int Col, int c, int b, int b_is_col, stats* stat_rel, int* original_relations,q_inter* inter);
int cost(Table* T,besttree* tree,stats** stat,pred* join_array, int count_of_joins,int* original_rel);
int get_query(char* buffer, pred* join_array, int count_of_joins, besttree* tree);
int set_is_connected(int* set,int set_size,pred* join_array, int size_join);
int Q_op(QueryArray* batch,Table* T);
#endif