//
//  partition.c
//  

#include <stdio.h>

//h main na kanei malloc ton reOrdered
/*---
reOrdered=malloc(sizeof(struct Relation));
reOrdered->tuples=malloc((endIndex-startIndex+1)*sizeof(struct tuple));
---*/
void Partition(struct Relation relation, int startIndex, int endIndex, int partitionStep, int numberOfBytes, struct Relation* reOrdered, int* max, int* pSum){
    
    int hist[pow(2,numberOfBytes)];  //isws na thelei malloc
    int currentIndex[pow(2,numberOfBytes)]; //boithitikos pinakas pou kratw se poia thesi prepei na topothetisw ton epomeno
    int hashedValue;
    
    pSum = malloc(pow(2,numberOfBytes)*sizeof(int));
    for(int i=0;i<pow(2,numberOfBytes;i++){
        hist[i]=0;
    }
    max=0;
    for(int j=startIndex;j<=endIndex;j++){
        hashedValue=relation.tuples[j].key>>((partitionStep-1)*numberOfBytes); // petaw ta teleutaia pshfia pou de xreiazomai
        hashedValue=hashedValue%(pow(2,numberOfBytes);  //petaw ta prwta pshfia pou de xreiazomai
        hist[hashedValue]++;
        if(hist[hashedValue]>max)
            max=hist[hashedValue];
    }
    pSum[0]=0;
    currentIndex[0]=startIndex;
    for(int i=1;i<pow(2,numberOfBytes;i++){
        pSum[i]=pSum[i-1]+hist[i-1];
        currentIndex[i]+=pSum[i];
    }
    for(int j=startIndex;j<=endIndex;j++){
        hashedValue=relation.tuples[j].key>>((partitionStep-1)*numberOfBytes); // petaw ta teleutaia pshfia pou de xreiazomai
        hashedValue=hashedValue%(pow(2,numberOfBytes);  //petaw ta prwta pshfia pou de xreiazomai
        reOrdered.tuples[currentIndex[hashedValue]].key=relation.tuples[j].key;
        reOrdered.tuples[currentIndex[hashedValue]].payload=relation.tuples[j].payload;
        currentIndex[hashedValue]++;
    }
}
