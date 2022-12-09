#include "../headers/structures.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

int TableFitsCache(int cacheSize, int tableSize, int offSet){ //offSet se kb
    
    if(tableSize>0 && ((cacheSize)-(offSet))>=tableSize*sizeof(struct tuple))
        return 1;
    else
        return 0;
};

int LoadTable(char *fileName,struct Table *table){
    int fd=open(fileName,O_RDONLY);
    struct stat sb;
    unsigned long fileSize;
    char* address;
    size_t numColumns;
    
    if(fd==-1){
        printf("Can not open %s file\n",fileName);
        return -1;
    }
    if(fstat(fd,&sb)==-1){
        printf("Can not open file. Fstat failed\n");
        return -1;
    }
    fileSize=sb.st_size;
    
    address=(char*)(mmap(NULL,fileSize,PROT_READ,MAP_PRIVATE,fd,0u));
    if(address==MAP_FAILED){
        printf("Can not mmap %s\n",fileName);
        return -1;
    }
    printf("here\n");
    //table=malloc(sizeof(struct Table));
    table->numRows=*((uint64_t*)(address));
    address+=sizeof(uint64_t);
    table->numColumns=*((size_t*)(address));
    //printf("numcolumns %ld\n",(*table)->numColumns);
    address+=sizeof(size_t);
    table->relations=malloc(table->numColumns*sizeof(uint64_t*));
    //printf("here\n");
    for(int i=0;i<table->numColumns;i++){
        table->relations[i]=malloc(sizeof(uint64_t)*table->numRows);
        /*table->relations[i]->num_tuples=table->numRows;
        table->relations[i]->tuples=malloc(table->numRows*sizeof(tuple));*/
        for(int j=0;j<table->numRows;j++){
            table->relations[i][j]=*((uint64_t*)(address));
            address+=sizeof(uint64_t);
        }
    }
};
