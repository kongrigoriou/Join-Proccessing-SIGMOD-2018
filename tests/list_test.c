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
    TEST_ASSERT(list->size >= 0);
    list_destroy_string(list);
}

void test_list_insert_string(void){
    List_string* list = list_create_string();
    
    TEST_ASSERT(list!=NULL);
    TEST_ASSERT(list->head == NULL);
    TEST_ASSERT(list->size == 0);
    list_insert_string(list,"insert string");
    TEST_ASSERT(list->size >= 0);
    list_destroy_string(list);
}

void test_list_print_string(void){
    List_string* list = list_create_string();
    
    TEST_ASSERT(list!=NULL);
    TEST_ASSERT(list->head == NULL);
    TEST_ASSERT(list->size == 0);
    
    list_insert_string(list,"testPrint");
    TEST_ASSERT(list->size >= 0);
    list_print_string(list);
    list_destroy_string(list);
}

void test_list_create(void){
    List* list = list_create();
    
    TEST_ASSERT(list->head == NULL);
    TEST_ASSERT(list->last == NULL);
    TEST_ASSERT(list->size == 0);
}

/*
 tuple* data=malloc(sizeof(tuple));
 data->key = 354729;
 data->payload = 962846;
 
 list_insert(list,data);
 TEST_ASSERT(list->size>=0);
 list_destroy(list);*/

TEST_LIST = {
    {"test_list_create_string",test_list_create_string},
    {"test_list_insert_string",test_list_insert_string},
    {"test_list_print_string",test_list_print_string},
    {"test_list_create",test_list_create},
    {NULL,NULL}
};

