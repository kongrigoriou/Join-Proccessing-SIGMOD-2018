#ifndef LIST
#define LIST

#include "../headers/structures.h"

typedef struct listnode{
    tuple data;
    struct listnode* next;
}ListNode;

typedef struct list{
    ListNode* head;
    ListNode* last;
    int size;
}List;

typedef struct listnode_string{
    char data[100];
    struct listnode_string* next;
}ListNode_string;

typedef struct list_string{
    ListNode_string * head;
    int size;
} List_string;

List_string* list_create_string();
void list_insert_string(List_string* , char* );
void list_destroy_string(List_string*);
void list_print_string(List_string* );

List* list_create();
void list_insert(List*, tuple);
void list_print(List*);
List* list_append(List*, List*);
void list_destroy(List*);

#endif

