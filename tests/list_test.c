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

void test_list_insert(void){
    List* list = list_create();
    
    TEST_ASSERT(list->head == NULL);
    TEST_ASSERT(list->last == NULL);
    TEST_ASSERT(list->size == 0);
    
    tuple* data=malloc(sizeof(tuple));
    data->key = 354729;
    data->payload = 962846;
    
    TEST_ASSERT(data->key == 354729);
    TEST_ASSERT(data->payload == 962846);
    
    list_insert(list,*data);
    TEST_ASSERT(list->size>=0);
    list_destroy(list);
}

void test_list_print(void){
    List* list = list_create();
    
    TEST_ASSERT(list->head == NULL);
    TEST_ASSERT(list->last == NULL);
    TEST_ASSERT(list->size == 0);
    
    tuple* data=malloc(sizeof(tuple));
    data->key = 354729;
    data->payload = 962846;
    
    TEST_ASSERT(data->key == 354729);
    TEST_ASSERT(data->payload == 962846);
    
    list_insert(list,*data);
    TEST_ASSERT(list->size>=0);
    list_print(list);
    list_destroy(list);
}

void test_list_append(void){
    List* new_list;
    List* new_list1 = list_create();
    
    tuple* data1=malloc(sizeof(tuple));
    data1->key = 354729;
    data1->payload = 962846;
    
    tuple* data2=malloc(sizeof(tuple));
    data1->key = 648372;
    data1->payload = 264822;
    
    list_insert(new_list1,*data1);
    list_insert(new_list1,*data2);
    TEST_ASSERT(new_list1->size == 2);
    
    List* new_list2 = list_create();
    
    tuple* data3=malloc(sizeof(tuple));
    data3->key = 542850;
    data3->payload = 736259;
    
    tuple* data4=malloc(sizeof(tuple));
    data4->key = 152738;
    data4->payload = 264822;
    
    list_insert(new_list2,*data3);
    list_insert(new_list2,*data4);
    TEST_ASSERT(new_list2->size == 2);
    
    new_list=list_append(new_list1,new_list2);
    
    TEST_ASSERT(new_list !=NULL);
    TEST_ASSERT(new_list->size == 4);
}

TEST_LIST = {
    {"test_list_create_string",test_list_create_string},
    {"test_list_insert_string",test_list_insert_string},
    {"test_list_print_string",test_list_print_string},
    {"test_list_create",test_list_create},
    {"test_list_insert",test_list_insert},
    {"test_list_print",test_list_print},
    {"test_list_append",test_list_append},
    {NULL,NULL}
};

