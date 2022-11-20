#include "../headers/list.h"
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

    // printf("Inserted: %d\n", data.key);

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

List* list_append(List* list1, List* list2){
    // list_print(list2);
    List* new_list = list_create();

    if(list1 == NULL){
        new_list->head = list2->head;
        new_list->size = list2->size;

        free(list2);
        return new_list;
    }

    if(list1->size == 0){
        new_list->head = list2->head;
        new_list->size += list2->size;

        free(list1);
        free(list2);
        list_print(new_list);
        return new_list;
    }

    ListNode* node = list1->head;

    while(node->next != NULL){
        node = node->next;
    }

    node->next = list2->head;
    list1->size += list2->size;

    new_list->head = list1->head;
    new_list->size = list1->size;
    free(list2);
    free(list1);

    return new_list;
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

