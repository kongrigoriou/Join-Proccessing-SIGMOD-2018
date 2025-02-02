//unit testing for hospschotch hashing
#include <stdio.h>
#include "../headers/acutest.h"
#include "../headers/hopscotch.h"
#include "../headers/bitmap.h"
#include <math.h>
#include "../headers/list.h"

void test_create(void){
    hop* array = create_array(10, 3);
    
    TEST_ASSERT(array->H == 3);
    TEST_ASSERT(array->size == 10);
    TEST_ASSERT(array->array != 0);

    destroy_hop(array);
}

void  test_hash(void){
    int hashvalue = hash(5, 5);
    TEST_ASSERT(hashvalue == hash(5,5));
}

void test_tuples(void){
    tuple* data = malloc(sizeof(*data));
    data->key = 279832;
    data->payload = 324983;

    TEST_ASSERT(data->key ==279832);
    TEST_ASSERT(data->payload == 324983);

    TEST_ASSERT(sizeof(tuple) == 2*sizeof(uint32_t));

    free(data);
}

void test_insert(void){
    hop* array = create_array(10, 3);
    int hash_value;

    tuple temp;
    temp.key = 2;
    temp.payload = 0;

    insert(array, temp,NULL);
    TEST_ASSERT(get_H(array) == 3);

    hash_value = hash(temp.key, array->size);
    TEST_ASSERT(array->array[hash_value].info.key == temp.key);

    temp.payload = 1;

    insert(array, temp,NULL);
    TEST_ASSERT(array->array[hash_value].duplicate->size == 1);

    temp.payload = 2;

    insert(array, temp,NULL);
    TEST_ASSERT(array->array[hash_value].duplicate->size == 2);

    // size = insert(array, temp,NULL);
    // TEST_ASSERT(size != 10);

    destroy_hop(array);
}

void test_search(void){
    hop* array = create_array(10, 3);
    List* results;

    tuple* tuple_array = malloc(5 * sizeof(tuple));

    for(int i = 0; i < 5; i++){
        tuple_array[i].key = i;
        tuple_array[i].payload = i;

        insert(array, tuple_array[i],NULL);
    }

    for(int i = 0; i < 5; i++){
        results = search(array, tuple_array[i]);
        TEST_ASSERT(results->head->data.payload == tuple_array[i].payload);
        list_destroy(results);
    }
    free(tuple_array);
    destroy_hop(array);
}

TEST_LIST = {
    {"test_create", test_create},
    {"test hash", test_hash},
    {"test_tuple", test_tuples},
    {"test insert", test_insert},
    {"test_search", test_search},
    {NULL, NULL}
};

