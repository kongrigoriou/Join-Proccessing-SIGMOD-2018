#include <stdio.h>
#include "../headers/acutest.h"
#include "../headers/queries_optimization.h"


void produce_next_set_test1(void){
    int set[]={0,0,0,0};
    produce_next_set(set,1,4);
    for(int i=0;i<4;i++){
        if(i!=0){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    produce_next_set(set,1,4);
    for(int i=0;i<4;i++){
        //printf("int %d\n",set[i]);
        if(i!=1){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    produce_next_set(set,1,4);
    for(int i=0;i<4;i++){
        //printf("int %d\n",set[i]);
        if(i!=2){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,1,4)!=NULL);
    for(int i=0;i<4;i++){
        //printf("int %d\n",set[i]);
        if(i!=3){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,1,4)==NULL);
    
}

void produce_next_set_test2(void){
    int set[]={0,0,0,0};
    TEST_ASSERT(produce_next_set(set,2,4)!=NULL);
    for(int i=0;i<4;i++){
        if(i!=0&&i!=1){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,2,4)!=NULL);
    for(int i=0;i<4;i++){
        //printf("int %d\n",set[i]);
        if(i!=0&&i!=2){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,2,4)!=NULL);
    for(int i=0;i<4;i++){
        if(i!=0&&i!=3){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,2,4)!=NULL);
    for(int i=0;i<4;i++){
        if(i!=1&&i!=2){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,2,4)!=NULL);
    for(int i=0;i<4;i++){
        if(i!=1&&i!=3){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,2,4)!=NULL);
    for(int i=0;i<4;i++){
        if(i!=2&&i!=3){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,2,4)==NULL);
}

void produce_next_set_test3(void){
    int set[]={0,0,0,0};
    TEST_ASSERT(produce_next_set(set,3,4)!=NULL);
    for(int i=0;i<4;i++){
        if(i!=0&&i!=1&&i!=2){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,3,4)!=NULL);
    for(int i=0;i<4;i++){
        if(i!=0&&i!=1&&i!=3){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,3,4)!=NULL);
    for(int i=0;i<4;i++){
        if(i!=0&&i!=2&&i!=3){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,3,4)!=NULL);
    for(int i=0;i<4;i++){
        
        if(i!=1&&i!=2&&i!=3){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,3,4)==NULL);
}

void produce_next_set_test1_3(void){
    int set[]={0,0,0,0};
    produce_next_set(set,1,3);
    for(int i=0;i<3;i++){
        if(i!=0){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    produce_next_set(set,1,3);
    for(int i=0;i<3;i++){
        if(i!=1){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    produce_next_set(set,1,3);
    for(int i=0;i<3;i++){
        if(i!=2){
            TEST_ASSERT(set[i]==0);
        }
        else{
            TEST_ASSERT(set[i]==1);
        }
    }
    TEST_ASSERT(produce_next_set(set,1,3)==NULL);
}

void produce_next_set_test3_3(void){
    int set[]={0,0,0,0};
    TEST_ASSERT(produce_next_set(set,3,3)!=NULL);
    for(int i=0;i<3;i++){
        
        TEST_ASSERT(set[i]==1);
    }
    TEST_ASSERT(produce_next_set(set,3,3)==NULL);
}

void get_num_test(void){
    int num[]={0,0,0,0};
    TEST_ASSERT(get_number(num,4)==0);
    num[0]=1;
    TEST_ASSERT(get_number(num,4)==1);
    num[1]=1;
    TEST_ASSERT(get_number(num,4)==3);
    num[0]=0;
    TEST_ASSERT(get_number(num,4)==2);
    num[3]=1;
    TEST_ASSERT(get_number(num,4)==10);
    TEST_ASSERT(get_number(num,3)==2);
}

TEST_LIST = {
    {"produce_next1",produce_next_set_test1},
    {"produce_next2",produce_next_set_test2},
    {"produce_next3",produce_next_set_test3},
    {"produce_next_1_3",produce_next_set_test1_3},
    {"produce_next_3_3",produce_next_set_test3_3},
    {"get_number_test",get_num_test},
    {NULL,NULL}
};