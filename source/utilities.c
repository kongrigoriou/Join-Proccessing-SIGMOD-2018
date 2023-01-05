#include "../headers/structures.h"
#include "../headers/bitmap.h"
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
    //size_t numColumns;
    
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
    //table=malloc(sizeof(struct Table));
    table->numRows=*((uint64_t*)(address));
    address+=sizeof(uint64_t);
    table->numColumns=*((size_t*)(address));
    //printf("numcolumns %ld\n",(*table)->numColumns);
    address+=sizeof(size_t);
    table->relations=malloc(table->numColumns*sizeof(uint64_t*));
    table->stats=malloc(table->numColumns*sizeof(stats));
    //printf("here\n");
    for(int i=0;i<table->numColumns;i++){
        table->relations[i]=malloc(sizeof(uint64_t)*table->numRows);
        table->stats[i].count=0;
        for(int j=0;j<table->numRows;j++){
            table->relations[i][j]=*((uint64_t*)(address));
            table->stats[i].count++;
            if(j==0){
                table->stats[i].max=table->relations[i][j];
                table->stats[i].min=table->relations[i][j];
            }
            if(table->stats[i].max < table->relations[i][j]){
                table->stats[i].max=table->relations[i][j];
            }
            if(table->stats[i].min > table->relations[i][j]){
                table->stats[i].min=table->relations[i][j];
            }
            address+=sizeof(uint64_t);
        }
        int number=table->stats[i].max-table->stats[i].min+1;
        if(number >50000000){
            table->stats[i].number=50000000;
        }
        else{
            table->stats[i].number=number;
        }
    }
    return 0;
};

//add index to hash array
int hash_dist(bitmap_t b, unsigned int number, uint64_t max, uint64_t min, uint64_t index){
    if(number==max-min+1){
        set_bit(b,index-min);
    }
    else{
        set_bit(b,index-min%number);
    }
    return 0;
}

int index_is_in_hash(bitmap_t b, unsigned int number, uint64_t max, uint64_t min, uint64_t index){
    int bit;
    if(number==max-min+1){
        bit=get_bit(b,index-min);
    }
    else{
        bit=get_bit(b,index-min%number);
    }
    return bit;
}


int fill_distinct_count(Table  *T, int table_size){

    for(int i=0;i<table_size;i++){
        for(int j=0;j<T[i].numColumns;j++){
            int number=T[i].stats[j].number;
            bitmap_t hash_of_col=create_bitmap(number);
            for(int k=0;k<T[i].numRows;k++){
                hash_dist(hash_of_col,T[i].stats[j].number,T[i].stats[j].max,T[i].stats[j].min, T[i].relations[j][k]);
            }
            T[i].stats[j].distinct_count=get_count(hash_of_col,number);
            //T[i].stats[j].distinct_hash=hash_of_col;
            free(hash_of_col);
        }
    }
    return 0;
}