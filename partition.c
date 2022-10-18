//
//  partition.c
//  

#include <stdio.h>

void Partition(struct Relation relation, int startIndex, int endIndex, int partitionStep, int numberOfBytes){
    
    int hist[2^numberOfBytes];  //isws na thelei malloc
    int pSum[2^numberOfBytes];
    int currentIndex[2^numberOfBytes]; //boithitikos pinakas pou kratw se poia thesi prepei na topothetisw ton epomeno
    int hashedValue,max=0;
    struct Relation* reOrdered;
    
    reOrdered=malloc(sizeof(struct Relation));
    reOrdered->tuples=malloc((endIndex-startIndex+1)*sizeof(struct tuple));
    
    for(int i=0;i<2^numberOfBytes;i++){
        hist[i]=0;
    }
    for(int j=startIndex;j<=endIndex;j++){
        // pare numberOfBytes * partitionStep
        hashedValue=relation.tuples[j].key>>((partitionStep-1)*numberOfBytes); // petaw ta teleutaia pshfia pou de xreiazomai
        hashedValue=hashedValue%(2^numberOfBytes);  //petaw ta prwta pshfia pou de xreiazomai
        hist[hashedValue]++;
        if(hist[hashedValue]>max)
            max=hist[hashedValue];
    }
    pSum[0]=0;
    currentIndex[0]=0;
    for(int i=1;i<2^numberOfBytes;i++){
        pSum[i]=pSum[i-1]+hist[i-1];
        currentIndex[i]=pSum[i];
    }
    for(int j=startIndex;j<=endIndex;j++){
        hashedValue=relation.tuples[j].key>>((partitionStep-1)*numberOfBytes); // petaw ta teleutaia pshfia pou de xreiazomai
        hashedValue=hashedValue%(2^numberOfBytes);  //petaw ta prwta pshfia pou de xreiazomai
        reOrdered.tuples[currentIndex[hashedValue]].key=relation.tuples[j].key;
        reOrdered.tuples[currentIndex[hashedValue]].payload=relation.tuples[j].payload;
        currentIndex[hashedValue]++;
    }
    
}
