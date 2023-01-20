#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../headers/structures.h"
#include "../headers/functions.h"
#include "../headers/queries_optimization.h"



void copy_stat_1(stats** stat1,stats* stat2, int stat2_size){
    *stat1=malloc(stat2_size*sizeof(stats));
    stats* stat=*stat1;
    for(int j=0;j<stat2_size;j++){
        stat[j].count =stat2[j].count;
        stat[j].distinct_count =stat2[j].distinct_count;
        stat[j].max =stat2[j].max;
        stat[j].min =stat2[j].min;
        stat[j].number =stat2[j].number;
    }
}


int exists_in_array(uint64_t* array,int size,int element){
    for(int i=0;i<size;i++){
        if(array[i]==element){
            return 1;
        }
    }
    return 0;
}

uint64_t maxim(uint64_t a,uint64_t b){
    if(a<b){
        return b;
    }
    else{
        return a;
    }
}

uint64_t minim(uint64_t a,uint64_t b){
    if(a>b){
        return b;
    }
    else{
        return a;
    }
}


int find_i_in_inter(q_inter* inter, int left){
    for(int i=0;i<inter->rel_count;i++){
        if(left==inter->relations[i]){
            return i;
        }
    }
    //should never get here
    return -1;
}

int update_filter(Table* T,int rel, int Col, int c, int b, int b_is_col, stats* stat_rel, int* original_relations,q_inter* inter){
    int original_i=original_relations[rel];
    uint64_t old_count=stat_rel[Col].count;
    //equality filter
    if(c=='='&&b_is_col==0){
        stat_rel[Col].max=b;
        stat_rel[Col].min=b;
        if(exists_in_array(T[original_i].relations[Col],T[original_i].numRows,b)){
            //printf("filter 1 col=%d\n",Col);
            stat_rel[Col].count=stat_rel[Col].count/(float)stat_rel[Col].distinct_count;
            stat_rel[Col].distinct_count=1;
        }
        else{
            //printf("filter 2\n");
            stat_rel[Col].count=0;
            stat_rel[Col].distinct_count=0;
        }
    }
    //range filters
    if(c!='='&&b_is_col==0){
        int k1=b;
        int k2=b;
        if(c=='<'){
            k1= stat_rel[Col].min;
            if(k2>stat_rel[Col].max){
                k2=stat_rel[Col].max;
            }
        }
        if(c=='>'){
            k2= stat_rel[Col].max;
            if(k1<stat_rel[Col].min){
                k2=stat_rel[Col].min;
            }
        }
        
        stat_rel[Col].distinct_count=(k2-k1)/(float)(stat_rel[Col].max-stat_rel[Col].min)*stat_rel[Col].distinct_count;
        stat_rel[Col].count=(k2-k1)/(float)(stat_rel[Col].max-stat_rel[Col].min)*stat_rel[Col].count;
        stat_rel[Col].min=k1;
        stat_rel[Col].max=k2;
    }
    //same filter
    if(b_is_col==1){
        //printf("I am here now\n");
        int max_min=maxim(stat_rel[Col].min,T[original_i].stats[b].min);
        int min_max=minim(stat_rel[Col].max,T[original_i].stats[b].max);
        int n=min_max-max_min+1;
        stat_rel[Col].min=max_min;
        stat_rel[b].min=max_min;
        stat_rel[Col].max=min_max;
        stat_rel[b].max=min_max;
        stat_rel[Col].count=old_count/(float)n;
        stat_rel[b].count=old_count/(float)n;
        uint64_t temp_num=stat_rel[Col].distinct_count*(1-pow(1-stat_rel[Col].count/old_count,stat_rel[Col].count/stat_rel[Col].distinct_count)) ;
        stat_rel[b].distinct_count=temp_num;
        stat_rel[Col].distinct_count=temp_num;
    }
    
    for(int i=0;i<T[original_i].numColumns;i++){
        //leave the column filter was applied to unchanged
        if(Col==i){
            continue;
        }
        //if b referes to column leave that alone as well
        if(b_is_col){
            if(b==i){
                continue;
            }
        }
        //printf("d[dc]=%ld,f[a]=%ld,f[old]=%ld d[c]=%ld\n",stat_rel[i].distinct_count,stat_rel[Col].count,old_count,stat_rel[i].count);
        uint64_t temp_num=stat_rel[i].distinct_count*(1-pow(1-stat_rel[Col].count/(float)old_count,stat_rel[i].count/(float)stat_rel[i].distinct_count)) ;
        stat_rel[i].distinct_count=temp_num;
        stat_rel[i].count=stat_rel[Col].count;
        
    }
    
    //if relation is in the intermidiate you need to update all results in the intermidiate
    if(inter!=NULL){
        for(int j=0;j<inter->rel_count;j++){
            for(int i=0;i<inter->columns[j];i++){
                //ignore the relation you just updated
                if(rel==i){
                    continue;
                }
                uint64_t temp_num=inter->stat[j][i].distinct_count*(1-pow(1-stat_rel[Col].count/(float)old_count,inter->stat[j][i].count/inter->stat[j][i].distinct_count)) ;
                inter->stat[j][i].distinct_count=temp_num;
                inter->stat[j][i].count=stat_rel[Col].count;
            }
        }
    }
    return 0;
}

int update_join(join_op j,q_inter* inter, stats* stat_right1, int stat_right_size){
    int left_stat_i=find_i_in_inter(inter,j.left);
    //printf("Stat i is %d\n",left_stat_i);
    stats* stat_left=inter->stat[left_stat_i];
    int left_Col=j.left_col;
    int right_Col=j.right_col;
    int right_rel=j.right;
    int left_rel=j.left;
    //printf("Before cpy in update join\n");
    //copy_stat_1(&inter.stat[inter.rel_count],stat_right1,stat_right_size);
    inter->stat[inter->rel_count]=malloc(stat_right_size*sizeof(stats));
    for(int j=0;j<stat_right_size;j++){
        inter->stat[inter->rel_count][j].count =stat_right1[j].count;
        inter->stat[inter->rel_count][j].distinct_count =stat_right1[j].distinct_count;
        inter->stat[inter->rel_count][j].max =stat_right1[j].max;
        inter->stat[inter->rel_count][j].min =stat_right1[j].min;
        inter->stat[inter->rel_count][j].number =stat_right1[j].number;
    }


    //printf("After cpy in update join\n");
    //printf("inter.stat[B]=%d\n",inter->stat[inter->rel_count][0].count);
    stats* stat_right=inter->stat[inter->rel_count];
    inter->relations[inter->rel_count]=right_rel;
    inter->columns[inter->rel_count]=stat_right_size;
    inter->rel_count++;
    

        
    
        //do an unofficial filter
        uint64_t filter_old_right_count=stat_right[right_Col].count;
        uint64_t filter_old_left_count=stat_left[left_Col].count;
        uint64_t old_right_count=stat_right[right_Col].distinct_count;
        uint64_t old_left_count=stat_left[left_Col].distinct_count;

        //printf("old stats_right: %ld %ld %ld %ld\n",stat_right[right_Col].count,stat_right[right_Col].distinct_count,stat_right[right_Col].max,stat_right[right_Col].min);
        //printf("old stats_left: %ld %ld %ld %ld\n",stat_left[left_Col].count,stat_left[left_Col].distinct_count,stat_left[left_Col].max,stat_left[left_Col].min);

        int max_min=maxim(stat_left[left_Col].min,stat_right[right_Col].min);
        int min_max=minim(stat_left[left_Col].max,stat_right[right_Col].max);

        stat_left[left_Col].count=(min_max-max_min)/(float)(stat_left[left_Col].max-stat_left[left_Col].min)*stat_left[left_Col].count;
        stat_left[left_Col].distinct_count=(min_max-max_min)/(float)(stat_left[left_Col].max-stat_left[left_Col].min)*stat_left[left_Col].distinct_count;
        stat_right[right_Col].count=(min_max-max_min)/(float)(stat_right[right_Col].max-stat_right[right_Col].min)*stat_right[right_Col].count;
        stat_right[right_Col].distinct_count=(min_max-max_min)/(float)(stat_right[right_Col].max-stat_right[right_Col].min)*stat_right[right_Col].distinct_count;

        stat_left[left_Col].min=max_min;
        stat_right[right_Col].min=max_min;
        stat_left[left_Col].max=min_max;
        stat_right[right_Col].max=min_max;

        for(int j=0;j<inter->rel_count;j++ ){
            for(int i=0;i<inter->columns[j];i++){
                if(left_rel==inter->relations[j] &&left_Col==i){
                    continue;
                }
                if(right_rel==inter->relations[j] &&right_Col==i){
                    continue;
                }
                uint64_t temp_num;
                
                if(right_rel==inter->relations[j]){
                    temp_num=stat_right[i].distinct_count*(1-pow(1-stat_right[right_Col].count/(float)filter_old_right_count,stat_right[i].count/(float)stat_right[i].distinct_count)) ;
                }
                else{
                    temp_num=inter->stat[j][i].distinct_count*(1-pow(1-stat_left[left_Col].distinct_count/(float)filter_old_left_count,inter->stat[j][i].count/(float)inter->stat[j][i].distinct_count)) ;
                }
                inter->stat[j][i].distinct_count=temp_num;
                inter->stat[j][i].count =stat_left[left_Col].count;
                

            }
        }
        
        //end of filter

        
        
    
        

        
        //printf("After_maxim and minimum\n");
        int n=min_max-max_min+1;
        
        

        stat_left[left_Col].count=(stat_left[left_Col].count*stat_right[right_Col].count)/(float)n; 
        stat_right[right_Col].count=stat_left[left_Col].count;

        stat_left[left_Col].distinct_count=(stat_left[left_Col].distinct_count *stat_right[right_Col].distinct_count)/(float)n; 
        stat_right[right_Col].distinct_count=stat_left[left_Col].distinct_count ;
        //printf("Before loop in join_update\n");
        //printf("new stats_right: %ld %ld %ld %ld\n",stat_right[right_Col].count,stat_right[right_Col].distinct_count,stat_right[right_Col].max,stat_right[right_Col].min);
        //printf("new stats_left: %ld %ld %ld %ld\n",stat_left[left_Col].count,stat_left[left_Col].distinct_count,stat_left[left_Col].max,stat_left[left_Col].min);
        for(int j=0;j<inter->rel_count;j++ ){
            for(int i=0;i<inter->columns[j];i++){
                if(left_rel==inter->relations[j] &&left_Col==i){
                    continue;
                }
                if(right_rel==inter->relations[j] &&right_Col==i){
                    continue;
                }
                
                uint64_t temp_num;
                
                if(right_rel==inter->relations[j]){
                    temp_num=stat_right[i].distinct_count*(1-pow(1-stat_right[right_Col].distinct_count/(float)old_right_count,stat_right[i].count/(float)stat_right[i].distinct_count)) ;
                }
                else{
                    temp_num=inter->stat[j][i].distinct_count*(1-pow(1-stat_left[left_Col].distinct_count/(float)old_left_count,inter->stat[j][i].count/(float)inter->stat[j][i].distinct_count)) ;
                }
                
                //printf("temp_num is %d\n",temp_num);
                inter->stat[j][i].distinct_count=temp_num;
                inter->stat[j][i].count =stat_left[left_Col].count;
                
            }
        }
    //}
    
        
    return 0;
}

//produce next set of size n (maximum size of set is 4)
int* produce_next_set(int* set,int n, int size){
    if(n==1){
        int first_1=-1;
        for(int i=0;i<4;i++){
            if(set[i]==1){
                first_1=i;
                break;
            }
        }
        if(first_1==-1){
            set[0]=1;
            return set;
        }
        if(first_1==size-1){
            return NULL;
        }
        set[first_1]=0;
        set[first_1+1]=1;
        return set;
    }

    if(n==2){
        int first_1=-1;
        int second_1=-1;
        for(int i=0;i<4;i++){
            if(set[i]==1){
                first_1=i;
                break;
            }
        }
        if(first_1==-1){
            set[0]=1;
            set[1]=1;
            return set;
        }
        for(int i=first_1+1;i<4;i++){
            if(set[i]==1){
                second_1=i;
                break;
            }
        }
        if(second_1==size-1){
            if(first_1==size-2){
                return NULL;
            }
            else{
                set[second_1]=0;
                set[first_1]=0;
                set[first_1+1]=1;
                set[first_1+2]=1;
            }
        }
        else{
            set[second_1]=0;
            set[second_1+1]=1;
        }
        return set;
    }
    if(n==3){
        int first_1=-1;
        int second_1=-1;
        int third_1=-1;
        for(int i=0;i<4;i++){
            if(set[i]==1){
                first_1=i;
                break;
            }
        }
        if(first_1==-1){
            set[0]=1;
            set[1]=1;
            set[2]=1;
            return set;
        }
        for(int i=first_1+1;i<4;i++){
            if(set[i]==1){
                second_1=i;
                break;
            }
        }
        for(int i=second_1+1;i<4;i++){
            if(set[i]==1){
                third_1=i;
                break;
            }
        }
        if(third_1==size-1){
            if(second_1==size-2&&first_1==size-3){
                return NULL;
            }
            if(second_1==size-2){
                set[third_1]=0;
                set[second_1]=0;
                set[first_1]=0;
                set[first_1+1]=1;
                set[first_1+2]=1;
                set[first_1+3]=1;
            }
            else{
                set[third_1]=0;
                set[second_1]=0;
                set[second_1+1]=1;
                set[second_1+2]=1;
            }
            return set;
        }
        else{
            set[third_1]=0;
            set[third_1+1]=1;
            return set;
        }
    }
    return NULL;
}

int get_number(int* array,int size){
    int sum=0;
    for(int i=0;i<size;i++){
        sum=sum+array[i]*pow(2,i);
    }
    return sum;
}

int connected(int j,int set[4],pred* join_array,int size_of_array){
    if(get_number(set,4)==0){
        return 1;
    }
    for(int i=0;i<size_of_array;i++){
        if(join_array[i].left_rel==j){
            if(set[join_array[i].right_rel]){
                return 1;
            }
        }
        if(join_array[i].right_rel==j){
            if(set[join_array[i].left_rel]){
                return 1;
            }
        }
    }
    return 0;
}

besttree* CreateJoinTree(besttree* tree,int j){
    //printf("in create join\n");
    besttree* CurrTree=malloc(sizeof(besttree));
    for(int i=0;i<tree->size;i++){
        CurrTree->relations[i]=tree->relations[i];
    }
    CurrTree->relations[tree->size]=j;
    CurrTree->size= tree->size+1;
    return CurrTree;
}

void copy_stats(Table* T,int count,int* original_relations, stats** stat){
    for(int i=0;i<count;i++){
        int T_i=original_relations[i];
        stat[i]=malloc(T[T_i].numColumns*sizeof(stats));
        for(int j=0;j<T[T_i].numColumns;j++){
            stat[i][j].count =T[T_i].stats[j].count;
            stat[i][j].distinct_count =T[T_i].stats[j].distinct_count;
            stat[i][j].max =T[T_i].stats[j].max;
            stat[i][j].min =T[T_i].stats[j].min;
            stat[i][j].number =T[T_i].stats[j].number;
        }
    }
}




int find_col(pred* join_array, int count_of_joins,int* tree, int tree_size,int B, join_op* array){
    int array_i=0;
    for(int j=0;j<tree_size;j++){
        for(int i=0;i<count_of_joins;i++){
            if(join_array[i].left_rel==tree[j] && join_array[i].right_rel==B){
                array[array_i].left=tree[j];
                array[array_i].left_col=join_array[i].left_col;
                array[array_i].right_col =join_array[i].right_col;
                array[array_i].right=B;
                array_i++;
            }
            if(join_array[i].right_rel==tree[j] && join_array[i].left_rel==B){
                array[array_i].left=tree[j];
                array[array_i].left_col=join_array[i].right_col;
                array[array_i].right_col =join_array[i].left_col;
                array[array_i].right=B;
                array_i++;
            }

        }
    }
    return array_i;
    
}

int cost(Table* T,besttree* tree,stats** stat,pred* join_array, int count_of_joins,int* original_rel){
    //printf("\nBegining of cost\n");
    q_inter* inter=malloc(sizeof(q_inter));
    //printf("where\n");
    inter->relations[0]=tree->relations[0];
    //printf("here\n");
    inter->rel_count=1;
    inter->columns[0]=T[original_rel[tree->relations[0]]].numColumns;
    //printf("Before cpy1\n");
    //copy_stat_1(&inter.stat[0],stat[tree->relations[0]],inter.columns[0]);
    inter->stat[0]=malloc(inter->columns[0]*sizeof(stats));
    for(int j=0;j<inter->columns[0];j++){
        inter->stat[0][j].count =stat[tree->relations[0]][j].count;
        inter->stat[0][j].distinct_count =stat[tree->relations[0]][j].distinct_count;
        inter->stat[0][j].max =stat[tree->relations[0]][j].max;
        inter->stat[0][j].min =stat[tree->relations[0]][j].min;
        inter->stat[0][j].number =stat[tree->relations[0]][j].number;
    }
    //printf("After cpy1\n");
    int sum_count=inter->stat[0][0].count;
    for(int i=1;i<tree->size-1;i++){
        //printf("start of loop i=%d\n",i);
        int B=tree->relations[i];
        join_op joins[10];
        find_col(join_array,count_of_joins,tree->relations,i,B,joins);
        //update stats
        //printf("Before update join j.righ=%d b=%d stat[B][0].count=%d\n",joins[0].right,B,stat[B][0].count);
        update_join(joins[0],inter,stat[B],T[original_rel[B]].numColumns);
        //printf("After update join\n");
        /*query_len=query_len+sprintf(&tree->query[query_len],"%d%d=%d%d ",joins[0].left,joins[0].left_col,joins[0].right,joins[0].right_col);
        for(int j=1;j<joins_count;j++){
            query_len=+sprintf(&tree->query[query_len],"%d%d=%d%d ",joins[j].left,joins[j].left_col,joins[j].right,joins[j].right_col);
            //update_filter(T,joins[i].right,joins[i].right_col)
            //filter  the rest of the joins that involve B and everyone else in intermidiate
        }*/
        sum_count+=inter->stat[i][joins[0].right_col].count;
    }
    //tree->query[query_len]='\0';
    //get size of intermidiate result
    //printf("before count\n");
    //int count=inter->stat[0][0].count;
    //printf("after count\n");
    for(int i=0;i<inter->rel_count;i++){

        free(inter->stat[i]);
        //printf("free loop i=%d\n",i);
    }
    free(inter);
    //printf("End\n");
    //tree->cost=sum_count;
    return sum_count;
}

int get_query(char* buffer, pred* join_array, int count_of_joins, besttree* tree){
    int buffer_size=0;
    for(int i=1;i< tree->size;i++){
        for(int k=0;k<i;k++){
            for(int j=0;j<count_of_joins;j++){
                if((join_array[j].left_rel==tree->relations[k]&&join_array[j].right_rel==tree->relations[i])|| (join_array[j].right_rel==tree->relations[k]&&join_array[j].left_rel==tree->relations[i])){
                    buffer_size=buffer_size+sprintf(&buffer[buffer_size],"%d%d=%d%d ",join_array[j].left_rel,join_array[j].left_col,join_array[j].right_rel,join_array[j].right_col);
                }
            }
        }
        
    }
    buffer[buffer_size]='\0';
    return 0;
}

int set_is_connected(int* set,int set_size,pred* join_array, int size_join){
    int temp_set[4]={0,0,0,0};
    for(int i=0;i<set_size;i++){
        temp_set[i]=set[i];
    }
    for(int i=0;i<set_size;i++){
        if(set[i]==0){
            continue;
        }
        temp_set[i]=0;
            
        if(!connected(i,temp_set,join_array,size_join)){
            return 0;
        }

        temp_set[i]=1;
    }
    return 1;
}

int Q_op(QueryArray* batch,Table* T){
    for(int j=0;j<batch->size;j++){
        //printf("\n\n\n query %d\n\n\n",j);
        QueryInfo* Q=batch->array[j];
        int count_of_filters=0;
        // printf("J: ")
        int size_r=Q->relationsCount;
        if(size_r<3){
            continue;
        }
        stats* stat[4];
                
        copy_stats(T,Q->relationsCount,Q->relationsId,stat);
        /*for(int i=0;i<size_r;i++){
            for(int j=0;j<T[Q->relationsId[i]].numColumns;j++){
                printf("Old Stat[%d][%d]count=%ld\n",i,j,stat[i][j].count);
                printf("Old Stat[%d][%d]dist=%ld\n",i,j,stat[i][j].distinct_count);
                printf("Old Stat[%d][%d]max=%ld\n",i,j,stat[i][j].max);
                printf("Old Stat[%d][%d]min=%ld\n\n",i,j,stat[i][j].min);
            }
        }*/     
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
            // printf("Filter:rel=%d col=%d comp=%c num=%d\n",rel,col,comp,num);
            //update stats
            update_filter(T,rel,col,comp,num,0,stat[rel],Q->relationsId,NULL);    
        }
        //print stats so far
        /*for(int i=0;i<size_r;i++){
            for(int j=0;j<T[Q->relationsId[i]].numColumns;j++){
                printf("Stat[%d][%d]count=%ld\n",i,j,stat[i][j].count);
                printf("Stat[%d][%d]dist=%ld\n",i,j,stat[i][j].distinct_count);
                printf("Stat[%d][%d]max=%ld\n",i,j,stat[i][j].max);
                printf("Stat[%d][%d]min=%ld\n\n",i,j,stat[i][j].min);
            }
        }*/
        int count_of_pred=0;
        //get the joins
        int size_of_join_array=Q->predicatesCount/6;
        pred* join_array=malloc(size_of_join_array*sizeof(pred));
        int step=0;
        while(count_of_pred<Q->predicatesCount){
            int left_rel=Q->predicates[count_of_pred]-48;
            int left_col=Q->predicates[count_of_pred+1]-48;
            int right_rel=Q->predicates[count_of_pred+3]-48;
            int right_col=Q->predicates[count_of_pred+4]-48;
            count_of_pred+=6;
            //join
            //printf("Join:left_rel=%d left_col=%d right_rel=%d right_col=%d\n",left_rel,left_col,right_rel,right_col);
            //get predicates
            join_array[step].left_rel=left_rel;
            join_array[step].left_col=left_col;
            join_array[step].right_rel=right_rel;
            join_array[step].right_col=right_col;
            step++; 
        }

        //start of algorithm
        //printf("Before the algorithm size_R=%d\n",size_r);
        besttree** best_tree=malloc(sizeof(besttree*)*pow(2,size_r));
        //int* nocrossproduct=calloc(pow(2,size_r),sizeof(int));
        for(int i=0;i<pow(2,size_r);i++){
            best_tree[i]=NULL;
        }
        //printf("after malloc loop\n");
        /*int temp[]={0,0,0,0};
        for(int i=0;i<size_r;i++){
                    
            temp[i]=1;
            int j=get_number(temp,size_r);
            best_tree[j]=malloc(sizeof(besttree));
            best_tree[j]->relations[0]=i;
            best_tree[j]->size=1;
            temp[i]=0;
        }*/
        int set1[]={0,0,0,0};
        for(int i=0;i<size_r;i++){
            produce_next_set(set1,1,size_r);
            besttree* node=malloc(sizeof(besttree));
            node->relations[0]=i;
            node->size=1;
            //printf("before best_tree d=%d\n",get_number(set1,size_r));
            best_tree[get_number(set1,size_r)]=node;
        }
        //printf("after first loop\n");
        for(int i=1;i<size_r;i++){
            int set[4]={0,0,0,0};
            //produce all sets of size i
            while(NULL!=produce_next_set(set,i,size_r)){
                /*for(int k=0;k<size_r;k++){
                    printf("Set[%d]=%d\n",k,set[k]);
                }*/
                for(int j=0;j<size_r;j++){
                    //printf("J is %d\n",j);
                    //if j exists in the set ignore it
                    if(set[j]==1){
                        continue;
                    }
                            
                    //if j isn't connected with a relation in the set ignore it
                    if(!connected(j,set,join_array,size_of_join_array)){
                        /*int temp[4];
                        for(int i=0;i<4;i++){
                            temp[i]=set[i];
                        }
                        temp[j]=1;
                        nocrossproduct[get_number(temp,size_r)]=1;*/
                        continue;
                    }
                    if(!set_is_connected(set,size_r,join_array,size_of_join_array)){
                        continue;
                    }
                    /*if(nocrossproduct[get_number(set,size_r)]){
                        break;
                    }*/
                    //printf("After connected number %d\n",get_number(set,size_r));
                    besttree* CurrTree=CreateJoinTree(best_tree[get_number(set,size_r)],j);
                    //printf("after join tree\n");
                    //create S'
                    int newset[4];
                    for(int i=0;i<4;i++){
                        newset[i]=set[i];
                    }
                    newset[j]=1;

                    //update bestTree if needed
                    /*for(int k=0;k<size_r;k++){
                        printf("newSet[%d]=%d\n",k,newset[k]);
                    }*/
                            
                    //printf("Number=%d\n",get_number(newset,size_r));
                    /*for(int k=0;k<size_r;k++){
                        printf("After new Set[%d]=%d\n",k,set[k]);
                    }*/
                    //int check=;
                    if(best_tree[get_number(newset,size_r)]==NULL||cost(T,best_tree[get_number(newset,size_r)],stat,join_array,size_of_join_array,Q->relationsId)>cost(T,CurrTree,stat,join_array,size_of_join_array,Q->relationsId) /*cost*/){
                        //printf("must enter here get_number=%d\n",get_number(newset,size_r));
                        if(best_tree[get_number(newset,size_r)]!=NULL){
                            free(best_tree[get_number(newset,size_r)]);
                        }
                        best_tree[get_number(newset,size_r)]=CurrTree;
                    }
                    else{
                        free(CurrTree);
                    }
                }
            }
            //printf("after produce loop\n");
        }
        int result_set[4]={1,1,1,1};
        //printf("%s\n",best_tree[get_number(result_set,size_r)]->query);
        char buffer_q[50];
        get_query(buffer_q,join_array,size_of_join_array,best_tree[get_number(result_set,size_r)]);
        //printf("Query=%s\n",Q->predicates);
        strcpy(Q->predicates,buffer_q);
        //printf("Query %d: %s\n",j+1,Q->predicates);
        /*for(int i=0;i<best_tree[get_number(result_set,size_r)]->size;i++){
            printf("tree[%d]=%d\n",i,best_tree[get_number(result_set,size_r)]->relations[i]);
        }*/
        free(join_array);
        for(int i=0;i<pow(2,size_r);i++){
            if(best_tree[i]!=NULL){
                free(best_tree[i]);
            }
        }
        free(best_tree);
        for(int i=0;i<size_r;i++){
            free(stat[i]);
        }
    }
    return 0;
}