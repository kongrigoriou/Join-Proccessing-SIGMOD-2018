#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./headers/structures.h"
#include "./headers/list.h"
#include "./headers/partitionedHashJoin.h"
#include "./headers/functions.h"
#include "./headers/queries.h"


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
    //printf("at least here\n");
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
    int** new_result =malloc(4*sizeof(int*));
    for(int i=0;i<4;i++){
        new_result[i]=calloc(size,sizeof(int));
    }
    printf("before for\n");
    for(int i=0;i<size;i++){
        //printf("[%d]=\n",i);
        for(int j=0;j<4;j++){
            if(j< interm->columns){
                new_result[j][i]=interm->inter[j][survivedIds[i]];
            }
        }
    }
    printf("after for\n");
    for(int i=0;i<4;i++){
        free(interm->inter[i]);
    }
    free(interm->inter);
    interm->inter=new_result;
    interm->rows=size;
    
}

//this will be called only when relations are in the same intermidiate
int filter_join(Table* T,joined** interm,int** RowIds,int* RowIdsize,int left_rel, int left_Col, int right_rel, int right_Col, int* original_relations,int left_inter,int right_inter){
    //get left relation's data
    int left_i=find_place(left_rel,*interm[left_inter]);
    int* left_ids=interm[left_inter]->inter[left_i];
    int original_left=original_relations[left_rel];                 //get original relation id
    Table T_left=T[original_left];                                  //get original relation
    uint64_t* data_left=T_left.relations[left_Col];                 //get relations

    //get right relation's data
    int right_i=find_place(right_rel,*interm[right_inter]);
    int* right_ids=interm[right_inter]->inter[right_i];
    int original_right=original_relations[right_rel];              //get original relation id
    Table T_right=T[original_right];                                  //get original relation
    uint64_t* data_right=T_right.relations[right_Col];              //get relations
    int* newrowids=malloc(sizeof(int)*interm[left_inter]->rows);
    int newrowids_index=0;
    printf("before if in filter join after all data has been extracted\n");
    if(left_inter==right_inter){
        for(int i=0;i<interm[left_inter]->rows;i++){
            if(data_left[left_ids[i]]==data_right[right_ids[i]]){
                newrowids[newrowids_index]=i;
                newrowids_index++;
            }
        }
        printf("before update join newrowids=%d and rows=%d\n",newrowids_index,interm[left_inter]->rows);
        update_joined(interm[left_inter],newrowids,newrowids_index);
        free(newrowids);
    }
    return 0;
}

int filter(Table* T,joined** interm,int** RowIds,int* RowIdsize,int rel, int Col, int c, int b, int b_is_col, int* original_relations){
    int exists_in_join=is_in_join(rel,interm);
    printf("\nexists=%d\n",exists_in_join);
    //relations isn't in intermidiate result
    if(exists_in_join==-1){
        if(RowIdsize[rel]==0){
            //nothing to do here
            return 0;
        }
        int* relIds=RowIds[rel];                                //current Rowids
        int* newrelIds=calloc(RowIdsize[rel],sizeof(int));      //new Rowids will be inserted here
        int j=0;                                                //actual size of new Rowids
        int original_i=original_relations[rel];                 //get original relation id
        Table T_i=T[original_i];                                //get original relation
        uint64_t* data=T_i.relations[Col];                      //get relations
        /*for(int i=0;i<RowIdsize[rel];i++){
            printf("Rowid[%d] of rel=%d is %d \n",i,rel,RowIds[rel][i]);
        }*/
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
        if(j==0){
            free(newrelIds);
            RowIds[rel]=NULL;
        }
        else{
            RowIds[rel]=newrelIds;
        }
        
        
        free(relIds);
    }
    //relation is in intermidiate result
    //not tested yet
    else{
        int rel_i=find_place(rel,*interm[exists_in_join]);
        int* relIds=interm[exists_in_join]->inter[rel_i];
        int relIds_size=interm[exists_in_join]->rows;
        if(relIds_size==0){
            //nothing to filter here
            return 0;
        }
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
    //joined** interm=*interm_;
    //if this is a self join
    if(left_rel==right_rel){
        filter(T,interm,RowIds,RowIdsize,left_rel,left_Col,'=',right_Col,1,original_relations);
        return 0;
    }
    int left_inter,right_inter;
    left_inter=is_in_join(left_rel,interm);
    right_inter=is_in_join(right_rel,interm);
    //left is in interm and right is not
    if(left_inter!=-1&&right_inter==-1){
        //create left relation
        printf("left is in inter right is not\n");
        int left_i=find_place(left_rel,*interm[left_inter]);
        printf("left_i is %d and should be 0\n",left_i);
        if(interm[left_inter]->rows==0){
            printf("in if of empty\n");
            int** new_result=NULL;
            interm[left_inter]->inter=new_result;
            interm[left_inter]->relations[interm[left_inter]->columns]=right_rel;
            interm[left_inter]->columns++;
            interm[left_inter]->rows=0;
            printf("end of if\n");
            return 0;
        }
        int* left_ids=interm[left_inter]->inter[left_i];
        printf("everything ok\n");
        
        int original_left=original_relations[left_rel];              //get original relation id
        Table T_left=T[original_left];                                  //get original relation
        printf("here? left_col=%d, original_left=%d\n",left_Col,original_left);
        uint64_t* data_left=T_left.relations[left_Col];              //get relations
        printf("actually here?\n");
        
        printf("after left\n");
        //create right relation
        int* right_ids=RowIds[right_rel];                                   //current Rowids
        int original_right=original_relations[right_rel];                   //get original relation id
        Table T_right=T[original_right];                                    //get original relation
        uint64_t* data_right=T_right.relations[right_Col];                        //get relations
        
        if(RowIdsize[right_rel]==0){
            for(int i=0;i< 4;i++){
                free(interm[left_inter]->inter[i]);
           
            }
            free(interm[left_inter]->inter);
            int** new_result=NULL;
            interm[left_inter]->inter=new_result;
            interm[left_inter]->relations[interm[left_inter]->columns]=right_rel;
            interm[left_inter]->columns++;
            interm[left_inter]->rows=0;
            return 0;
        }
        
        printf("before creation size=%d\n",interm[left_inter]->rows);
        relation left_relation= create_relation(left_ids,data_left,interm[left_inter]->rows,1);
        relation right_relation= create_relation(right_ids,data_right,RowIdsize[right_rel],0);
        //printf("Rowidsize=%d\n",RowIdsize[right_rel]);
        //print_relation("left_rel",left_relation);
        //print_relation("right_rel",right_relation);
        //join them together
        relation res=PartitionedHashJoin(&left_relation ,  &right_relation);
        free(right_relation.tuples);
        free(left_relation.tuples);
        //print_relation("res1",res);
        
        //create new intermidiate
        int** new_result=calloc(4,sizeof(int*));
        for(int i=0;i<4;i++){
            new_result[i]=calloc(res.num_tuples,sizeof(int));
        }
        printf("\nactive col=%d\n",interm[left_inter]->columns);
        for(int i=0;i<res.num_tuples;i++){
            for(int j=0;j<4;j++){
                if(j<interm[left_inter]->columns){  //for the relations that were already there
                    new_result[j][i]=interm[left_inter]->inter[j][res.tuples[i].key];
                    //printf("rel[%d]=%d\n",j,interm[left_inter]->relations[j]);
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
        printf("\nand of course %d",interm[left_inter]->relations[interm[left_inter]->columns-1]);
        free(res.tuples);
        //*interm_=interm;

    }
    //left is not in interm and right is
    else if(left_inter==-1&&right_inter!=-1){
        //create right relation
        printf("left relation is not in inermidiate and right is\n");
        int right_i=find_place(right_rel,*interm[right_inter]);
        
        //if there is nothing to be joined in right
        if(interm[right_inter]->rows==0){
            int** new_result=NULL;
            interm[right_inter]->inter=new_result;
            interm[right_inter]->relations[interm[right_inter]->columns]=left_rel;
            interm[right_inter]->columns++;
            interm[right_inter]->rows=0;
            return 0;
        }
        int* right_ids=interm[right_inter]->inter[right_i];
        
        int original_right=original_relations[right_rel];              //get original relation id
        Table T_right=T[original_right];                                  //get original relation
        uint64_t* data_right=T_right.relations[right_Col];              //get relations
    
        
        int* left_ids=RowIds[left_rel];                                   //current Rowids
        int original_left=original_relations[left_rel];                   //get original relation id
        Table T_left=T[original_left];                                    //get original relation
        uint64_t* data_left=T_left.relations[left_Col];                        //get relations
        
        if(RowIdsize[left_rel]==0){
            for(int i=0;i< 4;i++){
                free(interm[right_inter]->inter[i]);
           
            }
            free(interm[right_inter]->inter);
            int** new_result=NULL;
            interm[right_inter]->inter=new_result;
            interm[right_inter]->relations[interm[right_inter]->columns]=left_rel;
            interm[right_inter]->columns++;
            interm[right_inter]->rows=0;
            return 0;
        }
        
        //create relations
        relation right_relation= create_relation(right_ids,data_right,interm[right_inter]->rows,1);
        relation left_relation= create_relation(left_ids,data_left,RowIdsize[left_rel],0);
        
        //join them together
        relation res=PartitionedHashJoin(&right_relation ,  &left_relation);
        //print_relation("res0",res);
        free(right_relation.tuples);
        free(left_relation.tuples);
        //print_relation("res",res);
        
        //create new intermidiate
        int** new_result=calloc(4,sizeof(int*));
        for(int i=0;i<4;i++){
            new_result[i]=calloc(res.num_tuples,sizeof(int));
        }
        for(int i=0;i<res.num_tuples;i++){
            for(int j=0;j<4;j++){
                if(j<interm[right_inter]->columns){  //for the relations that were already there
                    new_result[j][i]=interm[right_inter]->inter[j][res.tuples[i].key];
                }
                if(j==interm[right_inter]->columns){ //for the relation to be added
                    new_result[j][i]=res.tuples[i].payload;
                }
                if(j>interm[right_inter]->columns){  //for the ralations that don't exist yet
                    new_result[j][i]=0;
                }
            }
        }

        //free previous interm
        for(int i=0;i< 4;i++){
            free(interm[right_inter]->inter[i]);
           
        }
        free(interm[right_inter]->inter);

        //update info
        interm[right_inter]->inter=new_result;
        interm[right_inter]->relations[interm[right_inter]->columns]=left_rel;
        interm[right_inter]->columns++;
        interm[right_inter]->rows=res.num_tuples;
        free(res.tuples);
    }
    

    //left is not in interm and right is not
    else if(left_inter==-1&&right_inter==-1){
        printf("No one in intermidiate yet\n");
        int* left_ids=RowIds[left_rel];                                   //current Rowids
        int original_left=original_relations[left_rel];                   //get original relation id
        Table T_left=T[original_left];                                    //get original relation
        uint64_t* data_left=T_left.relations[left_Col];                        //get relations
        printf("before left relation creation\n");
        
        

        int* right_ids=RowIds[right_rel];                                   //current Rowids
        int original_right=original_relations[right_rel];                   //get original relation id
        Table T_right=T[original_right];                                    //get original relation
        uint64_t* data_right=T_right.relations[right_Col];                        //get relations
        

        if(RowIdsize[left_rel]==0||RowIdsize[right_rel]==0){
            int interm_i;
            if(interm[0]==NULL){
                interm_i=0;
            }
            else{
                interm_i=1;
            }
            //create intermidiate
            printf("inter_i=%d\n",interm_i);
            int** new_result=NULL;
            interm[interm_i]= malloc(sizeof(joined));
            interm[interm_i]->relations=calloc(4,sizeof(int));
            interm[interm_i]->inter=new_result;
            interm[interm_i]->relations[0]=left_rel;
            interm[interm_i]->relations[1]=right_rel;
            interm[interm_i]->columns=2;
            interm[interm_i]->rows=0;
            return 0;
        }
        
        //join them together
        
        //printf("left relation size is %d\n",RowIdsize[left_rel]);
        //print_relation("right0",right_relation);
        //return 0;
        relation left_relation= create_relation(left_ids,data_left,RowIdsize[left_rel],0);
        printf("after left relation creation\n");
        relation right_relation= create_relation(right_ids,data_right,RowIdsize[right_rel],0);
        relation res=PartitionedHashJoin(&left_relation ,  &right_relation);
        printf("\nafter relation size=%ld\n",res.num_tuples);
        //print_relation("left0",left_relation);
        //print_relation("right0",right_relation);
        //print_relation("res0",res);
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
        printf("\nbefore filling up of interm\n");
        interm[interm_i]= malloc(sizeof(joined));
        interm[interm_i]->relations=calloc(4,sizeof(int));
        interm[interm_i]->inter=new_result;
        interm[interm_i]->relations[0]=left_rel;
        interm[interm_i]->relations[1]=right_rel;
        interm[interm_i]->columns=2;
        interm[interm_i]->rows=res.num_tuples;
        printf("after filling up of interm\n");
        free(res.tuples);
    }
    //both are in interm
    else{
        
        printf("both relations are in intermidiate\n");

        //if either of them is empty
        if(interm[left_inter]->rows==0||interm[right_inter]->rows==0){
            //and in different intermidiate add realations from intermidiate of right to inter of left and free right interm
            if(left_inter!=right_inter){
                

                //free the array of the non empty
                if(interm[left_inter]->rows==0){
                    for(int i=0;i<4;i++){
                        free(interm[left_inter]->inter[i]);
                    }
                }
                if(interm[left_inter]->rows==0){
                    for(int i=0;i<4;i++){
                        free(interm[right_inter]->inter[i]);
                    }
                }

                int** new_result=NULL;
                //merge left and right intermidiate in left
                interm[left_inter]->inter=new_result;
                for(int i=0;i<2;i++){
                    interm[left_inter]->relations[interm[left_inter]->columns]=interm[right_inter]->relations[i];
                    interm[left_inter]->columns++;
                }
                interm[left_inter]->rows=0;
                
                //free up right completely
                free(interm[right_inter]->inter);
                free(interm[right_inter]->relations);
                free(interm[right_inter]);
                interm[right_inter]=NULL;
                
                return 0;
            }
            //and they are both in the same intermidiate then do nothing
            else{
                return 0;
            }
        }
        

        //same interm
        if(left_inter==right_inter){
            printf("before filter join\n");
            filter_join(T,interm,RowIds,RowIdsize,left_rel,left_Col,right_rel,right_Col,original_relations,left_inter,right_inter);
        }
        //diff interm
        else{
            //to be added
        }
            
    }
    return 0;
}


int get_sum(Table* T,int rel,int col, joined** interm,int* original_rel){
    int interm_i=is_in_join(rel,interm);
    printf("interm_i in sum is %d\n",interm_i);
    int rowid_i=find_place(rel,*interm[interm_i]);
    int size =interm[interm_i]->rows;
    printf("size is %d\n",size);
    if(size==0){
        printf("NULL\n");
        return 0;
    }
    printf("where  is the problem rowid_i=%d\n",rowid_i);
    uint64_t sum=0;
    for(int i=0;i<size;i++){
        //printf("first loop\n");
        int index=interm[interm_i]->inter[rowid_i][i];
        //printf("before sum\n");
        sum=T[original_rel[rel]].relations[col][index]+sum;
    }
    printf("after loop sum:\n\n");
    printf("%ld",sum);
    printf("\n\n sum\n");
    return 0;
}



int main(int argc, char **argv){
   
    char* buffer=malloc(64);
    char* buffer1=malloc(64);
    size_t bufsize = 64;
    //size_t characters;
    printf("start\n");
    List_string* file_list=list_create_string();
    printf("\nwhat is happening\n");
    getline(&buffer,&bufsize,stdin);

    while(strcmp(buffer, "Done\n")){
        buffer[strlen(buffer)-1]='\0';
        printf("\nbuff=%s",buffer);
        snprintf(buffer1,64,"./input/small/%s",buffer);
        list_insert_string(file_list,buffer1);
        getline(&buffer,&bufsize,stdin);

    }
    free(buffer);
    free(buffer1);
    //list_print_string(file_list);
    Table* T=malloc(file_list->size*sizeof(Table));
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
    //int r[]={3,0,1};
    
    /*Table* T;
    T=malloc(3*sizeof(Table));
    T[0].numColumns=2;
    T[0].numRows=3;
    T[0].relations=malloc(sizeof(uint64_t*)*T[0].numColumns);
    for(int i=0;i<T[0].numColumns;i++){
        printf("in for T0\n");
        T[0].relations[i]=malloc(sizeof(uint64_t)*T[0].numRows);
        for(int j=0,l=0;j<T[0].numRows;j++){
            T[0].relations[i][j]=1;
        }
    }
    printf("after t0\n");
    T[1].numColumns=2;
    T[1].numRows=3;
    T[1].relations=malloc(sizeof(uint64_t*)*T[1].numColumns);
    for(int i=0;i<T[1].numColumns;i++){
        T[1].relations[i]=malloc(sizeof(uint64_t)*T[1].numRows);
        if(i==0){
            T[1].relations[i][0]=1;
            T[1].relations[i][1]=1;
            T[1].relations[i][2]=5;
        }
        else{
            
            T[1].relations[i][0]=10;
            T[1].relations[i][1]=20;
            T[1].relations[i][2]=30;
        }
    }
    T[2].numColumns=2;
    T[2].numRows=3;
    T[2].relations=malloc(sizeof(uint64_t*)*T[2].numColumns);
    for(int i=0;i<T[1].numColumns;i++){
        T[2].relations[i]=malloc(sizeof(uint64_t)*T[2].numRows);
        for(int j=0,l=0;j<T[2].numRows;j++){
            T[2].relations[i][j]=1;
        }
    }
    int table_size=3;*/

    //end of load Table
    //get queries
    
    Batches* queries;
    queries = get_query_info();
    print_queries(queries);
    for(int i=0;i<queries->size;i++){
        QueryArray* Batch_i= queries->batches[i];
        for(int j=0;j<Batch_i->size;j++){
            QueryInfo* Q=Batch_i->array[j];
            int count_of_filters=0;

            int size_r=Q->relationsCount;
            printf("sizeof(r)=%d\n",size_r);

            //initialize these arrays to run the filter and the join
            int** rowid=malloc(size_r*sizeof(int*));
            int* rowid_s=malloc(size_r*sizeof(int));
            joined* interm[2];
            //interm=malloc(sizeof(joined)*2);
            interm[0]=NULL;
            interm[1]=NULL;
            for(int i=0;i<size_r;i++){
                rowid_s[i]=T[Q->relationsId[i]].numRows;
                rowid[i]=calloc(T[Q->relationsId[i]].numRows,sizeof(int));
                printf("Rowid of rel=%d has %ld size\n",i, T[Q->relationsId[i]].numRows);
                for(int j=0;j<T[Q->relationsId[i]].numRows;j++){
                    rowid[i][j]=j;
                }
            }


            //get the filters
            while(count_of_filters<Q->filtersCount){
                
                int rel=Q->filters[count_of_filters]-48;        //-48
                int col=Q->filters[count_of_filters+1]-48;
                char comp=Q->filters[count_of_filters+2];
                int num=0;
                count_of_filters+=3;
                while(Q->filters[count_of_filters]!='|'){
                    num=num*10;
                    num=num+Q->filters[count_of_filters]-48;
                    count_of_filters++;
                }
                count_of_filters++;
                printf("Filter:rel=%d col=%d comp=%c num=%d\n",rel,col,comp,num);
                filter(T,interm,rowid,rowid_s,rel,col,comp,num,0,Q->relationsId);
            }
            int count_of_pred=0;
            //get the joins
            while(count_of_pred<Q->predicatesCount){
                int left_rel=Q->predicates[count_of_pred]-48;
                int left_col=Q->predicates[count_of_pred+1]-48;
                int right_rel=Q->predicates[count_of_pred+3]-48;
                int right_col=Q->predicates[count_of_pred+4]-48;
                count_of_pred+=6;
                //join
                printf("Join:left_rel=%d left_col=%d right_rel=%d right_col=%d\n",left_rel,left_col,right_rel,right_col);
                join(T,interm,rowid,rowid_s,left_rel,left_col,right_rel,right_col,Q->relationsId);
            }
            int count_of_sel=0;
            printf("\n");
            //get the sums
            while(count_of_sel< Q->selectionsCount ){
                if(count_of_sel!=0){
                    printf(" ");
                }
                int rel=Q->selections[count_of_sel]-48;
                int col=Q->selections[count_of_sel+1]-48;
                count_of_sel+=2;
                //sum
                printf("Sum:rel=%d col=%d\n",rel,col);
                //printf("[%d]=%d",count_of_sel,Q->selections[count_of_sel]);
                printf("\n");
                
                get_sum(T,rel,col,interm,Q->relationsId);
            }

            for(int i=0;i<size_r;i++){
                if(rowid[i]!=NULL){
                    free(rowid[i]);
                }
            }
            if(rowid!=NULL){
                free(rowid);
            }
            
            free(rowid_s);
            printf("after rowid's freed\n");
            if(interm[0]!=NULL){
                if(interm[0]->inter!=NULL){
                    for(int i=0;i< 4;i++ ){
                        if(interm[0]->inter[i]!=NULL){
                            free(interm[0]->inter[i]);
                        }
                    }
                    free(interm[0]->inter);
                    printf("please\n");
                }
                
                free(interm[0]->relations);
                printf("relesed everything\n");
                free(interm[0]);
            }
            if(interm[1]!=NULL){
                if(interm[1]->inter!=NULL){
                    for(int i=0;i< 4;i++ ){
                        if(interm[1]->inter[i]!=NULL){
                            free(interm[1]->inter[i]);
                        }
                    }
                    free(interm[1]->inter);
                    printf("please\n");
                }
                
                free(interm[1]->relations);
                printf("relesed everything\n");
                free(interm[1]);
            }
            free(Q->filters);
            free(Q->predicates);
            free(Q->selections);
            free(Q->relationsId);
            free(Q);
        }
        free(Batch_i->array);
        free(Batch_i);
    }
    free(queries->batches);
    free(queries);
    for(int i=0;i<table_size;i++){
        for(int j=0;j<T[i].numColumns;j++){
            free(T[i].relations[j]);
        }
        free(T[i].relations);
    }
    free(T);
    


    //small try
    /*Table* T;
    T=malloc(3*sizeof(Table));
    T[0].numColumns=2;
    T[0].numRows=100;
    T[0].relations=malloc(sizeof(uint64_t*)*T[0].numColumns);
    for(int i=0;i<T[0].numColumns;i++){
        printf("in for T0\n");
        T[0].relations[i]=malloc(sizeof(uint64_t)*T[0].numRows);
        for(int j=0,l=0;j<T[0].numRows;j++){
            T[0].relations[i][j]=l;
            l=l+2;
        }
    }
    printf("after t0");
    T[1].numColumns=2;
    T[1].numRows=3;
    T[1].relations=malloc(sizeof(uint64_t*)*T[1].numColumns);
    for(int i=0;i<T[1].numColumns;i++){
        T[1].relations[i]=malloc(sizeof(uint64_t)*T[1].numRows);
        if(i==0){
            T[1].relations[i][0]=1;
            T[1].relations[i][1]=1;
            T[1].relations[i][2]=5;
        }
        else{
            
            T[1].relations[i][0]=10;
            T[1].relations[i][1]=20;
            T[1].relations[i][2]=30;
        }
    }
    T[2].numColumns=2;
    T[2].numRows=100;
    T[2].relations=malloc(sizeof(uint64_t*)*T[2].numColumns);
    for(int i=0;i<T[1].numColumns;i++){
        T[2].relations[i]=malloc(sizeof(uint64_t)*T[2].numRows);
        for(int j=0,l=0;j<T[2].numRows;j++){
            T[2].relations[i][j]=l;
            l=l+2;
        }
    }
    int r[]={0,1,2};
    */
    
    /*int size_r=sizeof(r)/sizeof(int);
    printf("sizeof(r)=%d\n",size_r);
    int** rowid=malloc(size_r*sizeof(int*));
    int* rowid_s=malloc(size_r*sizeof(int));
    joined* interm[2];
    //interm=malloc(sizeof(joined)*2);
    interm[0]=NULL;
    interm[1]=NULL;
    for(int i=0;i<size_r;i++){
        rowid_s[i]=T[r[i]].numRows;
        rowid[i]=calloc(T[r[i]].numRows,sizeof(int));
        printf("Rowid of rel=%d has %ld size\n",i, T[r[i]].numRows);
        for(int j=0;j<T[r[i]].numRows;j++){
            rowid[i][j]=j;
        }
    }
    filter(T,interm,rowid,rowid_s,0,2,'>',3499,0,r);
    printf("\nafter filter\n");
    //join(T,interm,rowid,rowid_s,0,0,2,0,r); //for test
    join(T,interm,rowid,rowid_s,0,2,1,0,r);
    //printf("sum1=%ld and sum2=%ld\n",get_sum(T,0,1,interm),get_sum(T,1,1,interm));
    join(T,interm,rowid,rowid_s,0,1,2,0,r);
    
    printf("sum1=%ld and sum2=%ld\n",get_sum(T,1,2,interm,r),get_sum(T,0,1,interm,r));
    */
    /*relation r1;
    relation r2;
    r1.num_tuples=300;
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
    free(r2.tuples);*/
}
