//
//  mainPartitionTest.c


#include "mainPartitionTest.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int TableFitsCache(int cacheSize, int tableSize, int offSet);
int main(void){
    struct relation r1;
    struct relation* reOrdered;
    struct relation* reOrderedSecondStep;
    int tableFits,max,curMax;
    int* pSum;
    int* pSumSecondStep;
    
    r1.num_tuples=10;
    r1.tuples=malloc(10*sizeof(struct tuple));
    
    r1.tuples[0].key=1;
    r1.tuples[1].key=1;
    r1.tuples[2].key=9;
    r1.tuples[3].key=1;
    r1.tuples[4].key=1;
    r1.tuples[5].key=2;
    r1.tuples[6].key=2;
    r1.tuples[7].key=3;
    r1.tuples[8].key=1;
    r1.tuples[9].key=3;
    tableFits=TableFitsCache(15,10,5);
    if(tableFits==1){
        printf("O pinakas xwraei\n");
    }
    else{
        reOrdered=malloc(sizeof(struct relation));
        reOrdered->tuples=malloc(10*sizeof(struct tuple));
        max=0;
        Partition(r1,0,9,1,3,reOrdered,&max,&pSum);
        printf("reOrdered\n");
        for(int i=0;i<10;i++){
            printf("%d\n",reOrdered->tuples[i].key);
        }
        printf("pSum\n");
        for(int i=0;i<pow(2,3);i++){
            printf("%d\n",pSum[i]);
        }
        if(TableFitsCache(50,max,5)==1)
            printf("o pinakas xwraei me ena partition\n");
        else{
            reOrderedSecondStep=malloc(sizeof(struct relation));
            reOrderedSecondStep->tuples=malloc(10*sizeof(struct tuple));
            int j=0;
            max=0;
            while(j<pow(2,3)-1){
                if(pSum[j]!=pSum[j+1]){
                    Partition(*reOrdered,pSum[j],pSum[j+1]-1,2,3,reOrderedSecondStep,&curMax,&pSumSecondStep);
                    if(curMax>max)
                        max=curMax;
                    free(pSumSecondStep); //den xreiazomai allo ton pSumSecondStep
                }
                j++;
            }
            printf("o pinakas xwrese me 2 partition\n");
            for(int i=0;i<10;i++){
                printf("%d\n",reOrderedSecondStep->tuples[i].key);
            }
            //de xreiazetai na koitaksoume an xwraei - h ekfwnhsh mas stamata edw
            //free reOrderedSecondStep
        }
        //free reOrdered
    }
}

int TableFitsCache(int cacheSize, int tableSize, int offSet){ //offSet se kb
    
    if(tableSize>0 && ((cacheSize)-(offSet))>=tableSize*sizeof(struct tuple))
        return 1;
    else
        return 0;
};
