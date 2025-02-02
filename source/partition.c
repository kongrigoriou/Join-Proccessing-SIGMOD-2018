//
//  partition.c
//  
#include "../headers/mainPartitionTest.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void Partition(struct relation initialRelation, int startIndex, int endIndex, int partitionStep, int numberOfBytes, struct relation* reOrdered, int* max, int** pSum){
    
    int* hist;  //isws na thelei malloc
    int currentIndex[(int)pow(2,numberOfBytes)]; //boithitikos pinakas pou kratw se poia thesi prepei na topothetisw ton epomeno
    int hashedValue;
    
    *pSum = malloc((int)pow(2,numberOfBytes)*sizeof(int));
    hist=malloc((int)pow(2,numberOfBytes)*sizeof(int));
    for(int i=0;i<(int)pow(2,numberOfBytes);i++){
        hist[i]=0;
    }
    *max=0;
    for(int j=startIndex;j<=endIndex;j++){
        hashedValue=initialRelation.tuples[j].key>>((partitionStep-1)*numberOfBytes); // petaw ta teleutaia pshfia pou de xreiazomai
        hashedValue=hashedValue%((int)pow(2,numberOfBytes));  //petaw ta prwta pshfia pou de xreiazomai
        hist[hashedValue]++;
        if(hist[hashedValue]>(*max))
            (*max)=hist[hashedValue];
    }
    (*pSum)[0]=startIndex;
    currentIndex[0]=startIndex;
    for(int i=1;i<(int)pow(2,numberOfBytes);i++){
        (*pSum)[i]=(*pSum)[i-1]+hist[i-1];
        currentIndex[i]=(*pSum)[i];
    }
    for(int j=startIndex;j<=endIndex;j++){
        hashedValue=initialRelation.tuples[j].key>>((partitionStep-1)*numberOfBytes); // petaw ta teleutaia pshfia pou de xreiazomai
        hashedValue=hashedValue%((int)pow(2,numberOfBytes));  //petaw ta prwta pshfia pou de xreiazomai
        reOrdered->tuples[currentIndex[hashedValue]].key=initialRelation.tuples[j].key;
        reOrdered->tuples[currentIndex[hashedValue]].payload=initialRelation.tuples[j].payload;
        currentIndex[hashedValue]++;
    }
    free(hist);
}
