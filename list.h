#include "structures.h"

typedef struct node{
    int data;
    struct node* next;
}ListNode;

typedef struct list{
    ListNode* head;
    int size;
}List;

List* list_create();
void list_insert(List*, int);
void list_print(List*);
void list_destroy(List*);

