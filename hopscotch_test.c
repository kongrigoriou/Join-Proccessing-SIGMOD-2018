//unit testing for hospschotch hashing
#include <stdio.h>
#include "acutest.h"
#include "hopscotch.h"
#include "bitmap.h"
#include "math.h"
#include "list.h"

//test bitmap first
// void test_bitmap(void){
//     uint32_t array = 0;
//     uint32_t result = 0;

//     for(int i = 0; i < 8; i++){
//         set_bit((unsigned char *)&array, i);
//         result += pow(2, i);
//         TEST_ASSERT(array == result);
//     }

//     for(int i = 7; i >= 0; i--){
//         clear_bit((unsigned char *)&array, i);
//         result -= pow(2, i);
//         TEST_ASSERT(array == result);
//     }

// }

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
    int size;

    tuple temp;
    temp.key = 2;
    temp.payload = 0;

    size = insert(array, temp);

    hash_value = hash(temp.key, array->size);
    TEST_ASSERT(array->array[hash_value].info.key == temp.key);

    temp.payload = 1;

    size = insert(array, temp);
    TEST_ASSERT(array->array[hash_value + 1].info.payload == temp.payload);

    temp.payload = 2;

    size = insert(array, temp);
    TEST_ASSERT(array->array[hash_value + 2].info.payload == temp.payload);

    size = insert(array, temp);
    TEST_ASSERT(size != 10);
    destroy_hop(array);

}

TEST_LIST = {
    // {"test_bitmap", test_bitmap},
    {"test_create", test_create},
    {"test hash", test_hash},
    {"test_tuple", test_tuples},
    {"test insert", test_insert},
    {0, 0}
};

