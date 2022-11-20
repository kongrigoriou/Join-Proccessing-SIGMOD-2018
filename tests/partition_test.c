#include <stdio.h>
#include "../headers/acutest.h"
#include "../headers/mainPartitionTest.h"
#include "math.h"


//simple test with only one pass
void pass1_test1(void){
    struct relation r1,reordered;
    int max;
    int* pSum;
    r1.num_tuples=8;
    r1.tuples=malloc(8*sizeof(struct tuple));
    reordered.tuples=malloc(8*sizeof(struct tuple));
    for(int i=0,j=7;i<8;i++,j--){
        r1.tuples[i].payload=i;
        r1.tuples[i].key=j;
    }
    
    Partition(r1,0,7,1,3,&reordered,&max,&pSum);
    
    TEST_ASSERT(pSum[0]==0);
    TEST_ASSERT(pSum[1]==1);
    TEST_ASSERT(pSum[2]==2);
    TEST_ASSERT(pSum[3]==3);
    TEST_ASSERT(pSum[4]==4);
    TEST_ASSERT(pSum[5]==5);
    TEST_ASSERT(pSum[6]==6);
    TEST_ASSERT(pSum[7]==7);

    TEST_ASSERT(reordered.tuples[0].key==0 );
    TEST_ASSERT(reordered.tuples[1].key==1 );
    TEST_ASSERT(reordered.tuples[2].key==2 );
    TEST_ASSERT(reordered.tuples[3].key==3 );
    TEST_ASSERT(reordered.tuples[4].key==4 );
    TEST_ASSERT(reordered.tuples[5].key==5 );
    TEST_ASSERT(reordered.tuples[6].key==6 );
    TEST_ASSERT(reordered.tuples[7].key==7 );
    free(pSum);
    free(reordered.tuples);
    free(r1.tuples);
}

//simple test with only 1 pass
void pass1_test2(void){
    struct relation r1,reordered;
    int max;
    int* pSum;
    r1.num_tuples=2;
    r1.tuples=malloc(2*sizeof(struct tuple));
    reordered.tuples=malloc(2*sizeof(struct tuple));
    
    r1.tuples[0].key=1;
    r1.tuples[1].key=6;
    
    Partition(r1,0,1,1,3,&reordered,&max,&pSum);

    
    
    TEST_ASSERT(pSum[0]==0);
    TEST_ASSERT(pSum[1]==0);
    TEST_ASSERT(pSum[2]==1);
    TEST_ASSERT(pSum[3]==1);
    TEST_ASSERT(pSum[4]==1);
    TEST_ASSERT(pSum[5]==1);
    TEST_ASSERT(pSum[6]==1);
    TEST_ASSERT(pSum[7]==2);

    TEST_ASSERT(reordered.tuples[0].key==1 );
    TEST_ASSERT(reordered.tuples[1].key==6 );
    free(pSum);
    free(reordered.tuples);
    free(r1.tuples);
}

//simple test with 2 passes, similar to pass1_test1 in data
void pass2_test1(void){
    struct relation r1,reordered,newreord;
    int max;
    int* pSum;
    r1.num_tuples=16;
    r1.tuples=malloc(r1.num_tuples*sizeof(struct tuple));
    reordered.tuples=malloc(r1.num_tuples*sizeof(struct tuple));
    newreord.tuples=malloc(r1.num_tuples*sizeof(struct tuple));
    for(int i=0,j=r1.num_tuples-1;i<r1.num_tuples;i++,j--){
        r1.tuples[i].payload=i;
        r1.tuples[i].key=j;
        
    }
    
    Partition(r1,0,r1.num_tuples-1,1,3,&reordered,&max,&pSum);
    int** pSum1=malloc(pow(2,3)*sizeof(int*));
    int* pSumnew;
    
    int i=0;
    
    for(;i<pow(2,3)-1;i++){
        
        if(pSum[i]<pSum[i+1]){
            Partition(reordered,pSum[i],pSum[i+1]-1,2,3,&newreord,&max,&pSumnew);
            
            pSum1[i]=pSumnew;
        }
    }
    if(pSum[i]<r1.num_tuples){
        Partition(reordered,pSum[i],r1.num_tuples-1,2,3,&newreord,&max,&pSumnew);
        pSum1[i]=pSumnew;
    }
    int correct_sum=0;
    for(int i=0;i<pow(2,3);i++){
        for(int j=0;j<pow(2,3);j++){
            TEST_ASSERT(pSum1[i][j]==correct_sum );
            if(j==0||j==1){
                correct_sum++;
            }
            
        }
    }
    TEST_ASSERT(newreord.tuples[0].key==0 );
    TEST_ASSERT(newreord.tuples[1].key==8 );
    TEST_ASSERT(newreord.tuples[2].key==1 );
    TEST_ASSERT(newreord.tuples[3].key==9 );
    TEST_ASSERT(newreord.tuples[4].key==2 );
    TEST_ASSERT(newreord.tuples[5].key==10 );
    TEST_ASSERT(newreord.tuples[6].key==3 );
    TEST_ASSERT(newreord.tuples[7].key==11 );
    TEST_ASSERT(newreord.tuples[8].key==4 );
    TEST_ASSERT(newreord.tuples[9].key==12 );
    TEST_ASSERT(newreord.tuples[10].key==5 );
    TEST_ASSERT(newreord.tuples[11].key==13 );
    TEST_ASSERT(newreord.tuples[12].key==6 );
    TEST_ASSERT(newreord.tuples[13].key==14 );
    TEST_ASSERT(newreord.tuples[14].key==7 );
    TEST_ASSERT(newreord.tuples[15].key==15 );

    for(int i=0;i<pow(2,3);i++){
        free(pSum1[i]);
    }
    free(pSum1);
    free(pSum);
    //free(pSumnew);
    free(reordered.tuples);
    free(r1.tuples);
    free(newreord.tuples);
}

TEST_LIST = {
    // {"test_bitmap", test_bitmap},
    
    {"pass1_test1", pass1_test1},
    {"pass1_test2", pass1_test2},
    {"pass2_test1", pass2_test1},
    {NULL,NULL}
};