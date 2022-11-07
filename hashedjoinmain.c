#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "list.h"
#include "partitionedHashJoin.h"

void get_array(tuple** array,uint32_t * f_size ,tuple** s_array, uint32_t * s_size,char* name){
    FILE *myFile;
    int size;
    myFile = fopen(name, "r");
    //fscanf(myFile, "%1d", size);
    char buff[300];
    fgets(buff,sizeof(buff),myFile);
    char *p = strtok(buff," ");
    
    // int i=0;
    size=atoi(p);
    tuple* firstarray=malloc(sizeof(tuple)*size);
    p=strtok(NULL, " ");
    for(int i=0;i<size;i++){
        firstarray[i].key=atoi(p);
        firstarray[i].payload=i;
        p=strtok(NULL, " ");
    }

    *array=firstarray;
    *f_size=(uint32_t)size;
    fgets(buff,sizeof(buff),myFile);
    p = strtok(buff," ");
    
    // i=0;
    size=atoi(p);
    firstarray=malloc(sizeof(tuple)*size);
    p=strtok(NULL, " ");
    for(int i=0;i<size;i++){
        firstarray[i].key=atoi(p);
        firstarray[i].payload=i;
        p=strtok(NULL, " ");
    }
    *s_array=firstarray;
    *s_size=(uint32_t)size;
};

int main(int argc, char **argv){
    char *name = argv[1];
    struct relation first_r; 
    struct relation second_r;
    
    get_array(&first_r.tuples,&first_r.num_tuples, &second_r.tuples, &second_r.num_tuples , name);
    // for(int i=0;i<first_r.num_tuples;i++ ){
    //     printf("[%d]=%d\n\n",i,first_r.tuples[i].key);
    // }
    //printf("last element=%ld\n",sizeof(first_r.tuples)/sizeof(tuple) );
    List* result=PartitionedHashJoin(&second_r ,  &first_r);
    printf("\nMain print:\n");
    list_print(result);
    
    free(first_r.tuples);
    free(second_r.tuples);
}