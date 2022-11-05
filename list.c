#include "list.h"
#include <stdlib.h>
#include <stdio.h>

List* list_create(){
    List* list = malloc(sizeof(List));

    list->head = NULL;
    list->size = 0;

    return list;
}

void list_insert(List* list, tuple data){
    //create new node
    ListNode* newNode = malloc(sizeof(ListNode));
    
    newNode->data = data;
    
    if(list->size == 0){
        list->head = newNode;
        list->head->next = NULL;
    }else{
        newNode->next = list->head;
        list->head = newNode;        
    }

    printf("Inserted: %d\n", data.key);

    //update size
    list->size++;
}

void list_print(List* list){
    ListNode* node;
    node = list->head;
    for(int i = 0; i < list->size; i++){
        printf("%d %d\n", node->data.key, node->data.payload);
        node = node->next;
    }
}

void list_destroy(List* list){
    ListNode* node = list->head;

    while (node->next != NULL) {		
		ListNode* next = node->next;
        
        //free(node->data);
        free(node);
		node = next;
	}
    //free(node->data);
    free(node);
	free(list);
}

