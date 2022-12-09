#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./headers/structures.h"
#include "./headers/list.h"
#include "./headers/partitionedHashJoin.h"
#include "./headers/functions.h"


void get_array(tuple** array,uint64_t * f_size ,tuple** s_array, uint64_t * s_size,char* name){
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
    *f_size=(uint64_t)size;
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
    *s_size=(uint64_t)size;
    fclose(myFile);
};

//simple comparison function
int compare(int a, char c, int b){
    if(c=='='){
        return a==b;
    }
    if(c=='>'){
        return a>b;
    }
    if(c=='<'){
        return a<b;
    }
    return -1;
}


//find if relation exists in any intermidate result that you have(there can be maximum of 2 intermidiate results)
int is_in_join(int rel,joined** interm){
    printf("at least here\n");
    if(interm[0]!=NULL){
        int col=interm[0]->columns;
        for(int i=0;i<col;i++){
            if(interm[0]->relations[i]==rel){
                return 0;
            }
        }
    }
    if(interm[1]!=NULL){
        int col=interm[1]->columns;
        for(int i=1;i<col;i++){
            if(interm[1]->relations[i]==rel){
                return 1;
            }
        }
    }
    return -1;
}

//find rowIds of rel(ations) in intermidate result
int find_place(int rel,joined interm){
    for(int i=0;i<interm.columns;i++){
        if(interm.relations[i]==rel){
            return i;
        }
    }
    return -1;
}
//keep only the indexes that survived
void update_joined(joined* interm,int* survivedIds,int size){
    int** new_result =calloc(4,sizeof(int*));
    for(int i=0;i<4;i++){
        new_result=calloc(size,sizeof(int));
    }
    for(int i=0;i<size;i++){
        new_result[0][i]=interm->inter[0][survivedIds[i]];
        new_result[1][i]=interm->inter[0][survivedIds[i]];
        new_result[2][i]=interm->inter[0][survivedIds[i]];
        new_result[3][i]=interm->inter[0][survivedIds[i]];
    }
    for(int i=0;i<4;i++){
        free(interm->inter[i]);
    }
    free(interm->inter);
    interm->inter=new_result;
    interm->rows=size;
    
}

int filter(Table* T,joined** interm,int** RowIds,int* RowIdsize,int rel, int Col, int c, int b, int b_is_col, int* original_relations){
    int exists_in_join=is_in_join(rel,interm);
    printf("\nexists=%d\n",exists_in_join);
    //relations isn't in intermidiate result
    if(exists_in_join==-1){
        
        int* relIds=RowIds[rel];                                //current Rowids
        int* newrelIds=calloc(RowIdsize[rel],sizeof(int));      //new Rowids will be inserted here
        int j=0;                                                //actual size of new Rowids
        int original_i=original_relations[rel];                 //get original relation id
        Table T_i=T[original_i];                                //get original relation
        uint64_t* data=T_i.relations[Col];                      //get relations
        
        //if filter is isn't a weird join
        //meaning b will refer to an other column of the same relation
        if(b_is_col==0){
            for(int i=0;i<RowIdsize[rel];i++){
                if(compare(data[relIds[i]],c,b)){
                    newrelIds[j]=relIds[i];
                    j++;
                }
            }
        }
        else{
            uint64_t* data1=T_i.relations[b];                      //get second relation for self "join"
            for(int i=0;i<RowIdsize[rel];i++){
                if(compare(data[relIds[i]],c,data1[relIds[i]])){
                    newrelIds[j]=relIds[i];
                    j++;
                }
            }
            
        }
        //in any case store the result this way
        RowIdsize[rel]=j;
        RowIds[rel]=newrelIds;
        free(relIds);
    }
    //relation is in intermidiate result
    else{
        int rel_i=find_place(rel,*interm[exists_in_join]);
        int* relIds=interm[exists_in_join]->inter[rel_i];
        int relIds_size=interm[exists_in_join]->rows;
        int* newrelIds=calloc(relIds_size,sizeof(int));      //new Rowids will be inserted here
        int j=0;                                             //actual size of new Rowids
        int original_i=original_relations[rel];              //get original relation id
        Table T_i=T[original_i];                             //get original relation
        uint64_t* data=T_i.relations[Col];                   //get relations
        if(b_is_col==0){
            for(int i=0;i<relIds_size;i++){
                if(compare(data[relIds[i]],c,b)){
                    newrelIds[j]=i;                         //we want to know which indexes of the intermidiate result we will keep
                    j++;
                }
            }
        }
        else{
            uint64_t* data1=T_i.relations[b];                       //get second relation for self "join"
            for(int i=0;i<relIds_size;i++){
                if(compare(data[relIds[i]],c,data1[relIds[i]])){    //we want to know which indexes of the intermidiate result we will keep
                    newrelIds[j]=i;
                    j++;
                }
            }
            
        }
        //update joined result
        update_joined(interm[exists_in_join], newrelIds,j);
        free(relIds);
    }
    
  return 0;  
}

relation create_relation(int* indexes, uint64_t* data,int ind_size,int j){
    relation r;
    r.num_tuples=ind_size;
    r.tuples=calloc(ind_size,sizeof(tuple));
    if(j){
        for(int i=0;i<ind_size;i++){
            r.tuples[i].key=data[indexes[i]];
            r.tuples[i].payload=i;
        }
    }
    else{
        for(int i=0;i<ind_size;i++){
            r.tuples[i].key=data[indexes[i]];
            r.tuples[i].payload=indexes[i];
        }
    }
    return r;
}

void print_relation(char* name, relation rel){
    for(int i=0;i<rel.num_tuples; i++){
        printf("%s[%d]= key=%ld | payload=%ld\n",name,i ,rel.tuples[i].key, rel.tuples[i].payload );
    }
}


int join(Table* T,joined** interm,int** RowIds,int* RowIdsize,int left_rel, int left_Col, int right_rel, int right_Col,int* original_relations){
    int left_inter,right_inter;
    left_inter=is_in_join(left_rel,interm);
    right_inter=is_in_join(right_rel,interm);
    //left is in interm and right is not
    if(left_inter!=-1&&right_inter==-1){
        //create left relation
        int left_i=find_place(left_rel,*interm[left_inter]);
        int* left_ids=interm[left_inter]->inter[left_i];
        int original_left=original_relations[left_rel];              //get original relation id
        Table T_left=T[original_left];                                  //get original relation
        uint64_t* data_left=T_left.relations[left_Col];              //get relations
        relation left_relation= create_relation(left_ids,data_left,interm[left_rel]->rows,1);

        //create right relation
        int* right_ids=RowIds[right_rel];                                   //current Rowids
        int original_right=original_relations[right_rel];                   //get original relation id
        Table T_right=T[original_right];                                    //get original relation
        uint64_t* data_right=T_right.relations[right_Col];                        //get relations
        relation right_relation= create_relation(right_ids,data_right,RowIdsize[right_rel],0);

        //join them together
        relation res=PartitionedHashJoin(&left_relation ,  &right_relation);
        free(right_relation.tuples);
        free(left_relation.tuples);

        
        //create new intermidiate
        int** new_result=calloc(4,sizeof(int*));
        for(int i=0;i<4;i++){
            new_result[i]=calloc(res.num_tuples,sizeof(int));
        }
        for(int i=0;i<res.num_tuples;i++){
            for(int j=0;j<4;j++){
                if(j<interm[left_inter]->columns){  //for the relations that were already there
                    new_result[j][i]=interm[left_inter]->inter[j][res.tuples[i].key];
                }
                if(j==interm[left_inter]->columns){ //for the relation to be added
                    new_result[j][i]=res.tuples[i].payload;
                }
                if(j>interm[left_inter]->columns){  //for the ralations that don't exist yet
                    new_result[j][i]=0;
                }
            }
        }
        //free previous interm
        for(int i=0;i< 4;i++){
            free(interm[left_inter]->inter[i]);
           
        }
        free(interm[left_inter]->inter);
        //update info
        interm[left_inter]->inter=new_result;
        interm[left_inter]->relations[interm[left_inter]->columns]=right_rel;
        interm[left_inter]->columns++;
        interm[left_inter]->rows=res.num_tuples;

    }
    //left is not in interm and right is
    else if(left_inter==-1&&right_inter!=-1){

    }
    

    //left is not in interm and right is not
    else if(left_inter==-1&&right_inter==-1){
        printf("No one in intermidiate yet\n");
        int* left_ids=RowIds[left_rel];                                   //current Rowids
        int original_left=original_relations[left_rel];                   //get original relation id
        Table T_left=T[original_left];                                    //get original relation
        uint64_t* data_left=T_left.relations[left_Col];                        //get relations
        printf("before left relation creation\n");
        relation left_relation= create_relation(left_ids,data_left,RowIdsize[left_rel],0);
        printf("after left relation creation\n");

        int* right_ids=RowIds[right_rel];                                   //current Rowids
        int original_right=original_relations[right_rel];                   //get original relation id
        Table T_right=T[original_right];                                    //get original relation
        uint64_t* data_right=T_right.relations[right_Col];                        //get relations
        relation right_relation= create_relation(right_ids,data_right,RowIdsize[right_rel],0);
        
        //join them together
        print_relation("left",left_relation);
        print_relation("right",right_relation);
        //return 0;
        relation res=PartitionedHashJoin(&left_relation ,  &right_relation);
        free(right_relation.tuples);
        free(left_relation.tuples);

        int interm_i;
        if(interm[0]==NULL){
            interm_i=0;
        }
        else{
            interm_i=1;
        }
        //create intermidiate
        int** new_result=calloc(4,sizeof(int*));
        for(int i=0;i<4;i++){
            new_result[i]=calloc(res.num_tuples,sizeof(int));
        }
        for(int i=0;i<res.num_tuples;i++){
            for(int j=0;j<4;j++){
                if(j==0){  //for the relations that were already there
                    new_result[j][i]=res.tuples[i].key;
                }
                else if (j==1){
                    new_result[j][i]=res.tuples[i].payload;
                }
                else{
                    new_result[j][i]=0;
                }
            }
        }
        interm[interm_i]=calloc(sizeof(joined),1);
        interm[left_inter]->inter=new_result;
        interm[left_inter]->relations[0]=left_rel;
        interm[left_inter]->relations[1]=right_rel;
        interm[left_inter]->columns=2;
        interm[left_inter]->rows=res.num_tuples;

    }
    //both are in interm
    else{
        //same interm
        if(left_inter==right_inter){

        }
        //diff interm
        else{

        }
    }
    return 0;
}


uint64_t get_sum(Table* T,int rel,int col, joined** interm){
    int interm_i=is_in_join(rel,interm);
    int rowid_i=find_place(rel,*interm[interm_i]);
    int size =interm[interm_i]->rows;
    uint64_t sum=0;
    for(int i=0;i<size;i++){
        int index=interm[interm_i]->inter[rowid_i][i];
        sum+=T[rel].relations[col][index];
    }
    return sum;
}

int main(int argc, char **argv){
   /* char *name = argv[1];
    struct relation first_r; 
    struct relation second_r;
    
    get_array(&first_r.tuples,&first_r.num_tuples, &second_r.tuples, &second_r.num_tuples , name);
    // for(int i=0;i<first_r.num_tuples;i++ ){
    //     printf("[%d]=%d\n\n",i,first_r.tuples[i].key);
    // }
    //printf("last element=%ld\n",sizeof(first_r.tuples)/sizeof(tuple) );
    //List* result=PartitionedHashJoin(&first_r ,  &second_r);
    printf("\nMain print:\n");
    list_print(result);
    
    list_destroy(result);
    free(first_r.tuples);
    free(second_r.tuples);*/
    /*char* buffer=malloc(64);
    char* buffer1=malloc(64);
    size_t bufsize = 64;
    //size_t characters;
    printf("start\n");
    List_string* file_list=list_create_string();
    printf("\nwhat is happening\n");
    while( -1!=getline(&buffer,&bufsize,stdin)){
        buffer[strlen(buffer)-1]='\0';
        printf("\nbuff=%s",buffer);
        snprintf(buffer1,64,"./input/small/%s",buffer);
        list_insert_string(file_list,buffer1);
    }
    free(buffer);
    free(buffer1);
    list_print_string(file_list);
    Table* T=calloc(file_list->size,sizeof(Table));
    ListNode_string* node=file_list->head;
    int table_size=file_list->size;
    for(int i=table_size-1;i>=0;i--){
        printf("\ni=%d\n",i);
        LoadTable(node->data,&T[i]);
        node=node->next;
    }
    list_destroy_string(file_list);
    for(int i=0;i<table_size;i++){
        printf("\nrow=%ld col=%ld i[0][0]=%ld\n",T[i].numRows, T[i].numColumns,T[i].relations[0][0]);
    }
    //end of load Table
    
    int r[]={3,0,1};
    int size_r=sizeof(r)/sizeof(int);
    printf("sizeof(r)=%d\n",size_r);
    int** rowid=malloc(size_r);
    int* rowid_s=malloc(size_r);
    joined* interm[2];
    interm[0]=NULL;
    interm[1]=NULL;
    for(int i=0;i<size_r;i++){
        rowid_s[i]=T[r[i]].numRows;
        rowid[i]=calloc(T[r[i]].numRows,sizeof(int));
        for(int j=0;j<T[r[i]].numRows;j++){
            rowid[i][j]=j;
        }
    }
    filter(T,interm,rowid,rowid_s,0,2,'>',3499,0,r);
    printf("\nafter filter\n");
    join(T,interm,rowid,rowid_s,0,2,1,0,r);
    join(T,interm,rowid,rowid_s,0,1,2,0,r);*/
    relation r1;
    relation r2;
    r1.num_tuples=380;
    r1.tuples=malloc(r1.num_tuples*sizeof(tuple));
    r2.num_tuples=1;
    r2.tuples=malloc(r2.num_tuples*sizeof(tuple));
    for(int i=0;i<r1.num_tuples;i++){
        if(i<r2.num_tuples){
            r2.tuples[i].key=1;
            r2.tuples[i].payload=i;
        }
        r1.tuples[i].key=1;
        r1.tuples[i].payload=i;
    }
    PartitionedHashJoin(&r1,&r2);
    free(r1.tuples);
    free(r2.tuples);
}
