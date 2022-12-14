#include <stdio.h>
#include "../headers/acutest.h"
#include "../headers/list.h"
#include <stdlib.h>
#include <string.h>

void test_list_create_string(void){
    List_string* list = list_create_string();
    
    TEST_ASSERT(list!=NULL);
    TEST_ASSERT(list->head == NULL);
    TEST_ASSERT(list->size == 0);
    
    list_insert_string(list,"test");
    TEST_ASSERT(list->size == 1);
    TEST_ASSERT(!strcmp(list->head->data,"test"));
    list_destroy_string(list);
}

void test_list_insert_string(void){
    List_string* list = list_create_string();
    
    TEST_ASSERT(list!=NULL);
    TEST_ASSERT(list->head == NULL);
    TEST_ASSERT(list->size == 0);
    list_insert_string(list,"insert string");
    TEST_ASSERT(list->size == 1);
    list_destroy_string(list);
}


void test_list_create(void){
    List* list = list_create();
    
    TEST_ASSERT(list->head == NULL);
    TEST_ASSERT(list->last == NULL);
    TEST_ASSERT(list->size == 0);
    list_destroy(list);
}

void test_list_insert(void){
    List* list = list_create();
    
    TEST_ASSERT(list->head == NULL);
    TEST_ASSERT(list->last == NULL);
    TEST_ASSERT(list->size == 0);
    
    tuple* data=malloc(sizeof(tuple));
    data->key = 354729;
    data->payload = 962846;
    
    list_insert(list,*data);
    TEST_ASSERT(list->size==1);

    TEST_ASSERT(list->head->data.key==354729);
    TEST_ASSERT(list->head->data.payload==962846);
    TEST_ASSERT(list->last->data.key==354729);
    TEST_ASSERT(list->last->data.payload==962846);
    list_destroy(list);
    free(data);
}

void test_list_append(void){
    List* list = list_create();
    List* list1 = list_create();
    
    tuple data;
    data.key = 354729;
    data.payload = 962846;
    

    
    list_insert(list,data);
    list1=list_append(list1,list);
    TEST_ASSERT(list1->size==1);
    TEST_ASSERT(list1->head->data.key=354729);
    TEST_ASSERT(list1->head->data.payload=962846);
    TEST_ASSERT(list1->last->data.key=354729);
    TEST_ASSERT(list1->last->data.payload=962846);
    list_destroy(list1);
}

TEST_LIST = {
    {"test_list_create_string",test_list_create_string},
    {"test_list_insert_string",test_list_insert_string},
    {"test_list_create",test_list_create},
    {"test_list_insert",test_list_insert},
    {"test_list_append",test_list_append},
    {NULL,NULL}
};

