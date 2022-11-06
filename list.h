#ifndef LIST
#define LIST

#include "structures.h"

typedef struct listnode{
    tuple data;
    struct listnode* next;
}ListNode;

typedef struct list{
    ListNode* head;
    int size;
}List;

List* list_create();
void list_insert(List*, tuple);
void list_print(List*);
List* list_append(List*, List*);
void list_destroy(List*);

#endif

