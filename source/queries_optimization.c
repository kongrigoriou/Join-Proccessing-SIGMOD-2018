#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../headers/structures.h"
#include "../headers/functions.h"
#include <math.h>

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

int update_filter(Table* T,joined** interm,int rel, int Col, int c, int b, int b_is_col, int* original_relations){
    int original_i=original_relations[rel];
    int old_count=T[original_i].stats[c].count;
    //equality filter
    if(c=='='&&b_is_col==0){
        T[original_i].stats[c].max=b;
        T[original_i].stats[c].min=b;
        if(exists_in_array(T[original_i].relations[c],T[original_i].numRows,b)){
            T[original_i].stats[c].count=T[original_i].stats[c].count/T[original_i].stats[c].distinct_count;
            T[original_i].stats[c].distinct_count=1;
        }
        else{
            T[original_i].stats[c].count=0;
            T[original_i].stats[c].distinct_count=0;
        }
    }
    //range filters
    if(c!='='&&b_is_col==0){
        int k1=b;
        int k2=b;
        if(c=='<'){
            k1= T[original_i].stats[c].min;
            if(k2>T[original_i].stats[c].max){
                k2=T[original_i].stats[c].max;
            }
        }
        if(c=='>'){
            k2= T[original_i].stats[c].max;
            if(k1<T[original_i].stats[c].min){
                k2=T[original_i].stats[c].min;
            }
        }
        T[original_i].stats[c].min=k1;
        T[original_i].stats[c].max=k2;
        T[original_i].stats[c].distinct_count=(k2-k1)/(T[original_i].stats[c].max-T[original_i].stats[c].min)*T[original_i].stats[c].distinct_count;
        T[original_i].stats[c].count=(k2-k1)/(T[original_i].stats[c].max-T[original_i].stats[c].min)*T[original_i].stats[c].count;
    }
    //same filter
    if(b_is_col==1){
        int max_min=maxim(T[original_i].stats[c].min,T[original_i].stats[b].min);
        int min_max=minim(T[original_i].stats[c].max,T[original_i].stats[b].max);
        int n=min_max-max_min+1;
        T[original_i].stats[c].min=max_min;
        T[original_i].stats[b].min=max_min;
        T[original_i].stats[c].max=min_max;
        T[original_i].stats[b].max=min_max;
        T[original_i].stats[c].count=old_count/n;
        T[original_i].stats[b].count=old_count/n;
        int temp_num=T[original_i].stats[c].distinct_count*(1-pow(1-T[original_i].stats[c].count/old_count,T[original_i].stats[c].count/T[original_i].stats[c].distinct_count)) ;
        T[original_i].stats[b].distinct_count=temp_num;
        T[original_i].stats[c].distinct_count=temp_num;
    }
    for(int i=0;i<T[original_i].numColumns;i++){
        //leave the collumn filter was aplied to unchanged
        if(c==i){
            continue;
        }
        //if b referes to column leave that alone as well
        if(b_is_col){
            if(b==i){
                continue;
            }
        }
        int temp_num=T[original_i].stats[i].distinct_count*(1-pow(1-T[original_i].stats[c].count/old_count,T[original_i].stats[i].count/T[original_i].stats[i].distinct_count)) ;
        T[original_i].stats[i].distinct_count=temp_num;
        T[original_i].stats[i].count=T[original_i].stats[c].count;
        
    }
    return 0;

}

int update_join(Table* T,joined** interm,int left_rel, int left_Col, int right_rel, int right_Col,int* original_relations){
    int original_left=original_relations[left_rel];
    int original_right=original_relations[right_rel];
    if(left_rel!=right_rel){
        int old_left_count=T[original_left].stats[left_Col].count;
        int old_right_count=T[original_right].stats[right_Col].count;

        int max_min=maxim(T[original_left].stats[left_Col].min,T[original_right].stats[right_Col].min);
        int min_max=minim(T[original_left].stats[left_Col].max,T[original_right].stats[right_Col].max);
        int n=min_max-max_min+1;
        T[original_left].stats[left_Col].min=max_min;
        T[original_right].stats[right_Col].min=max_min;
        T[original_left].stats[left_Col].max=min_max;
        T[original_right].stats[right_Col].max=min_max;

        T[original_left].stats[left_Col].count=(T[original_left].stats[left_Col].count*T[original_right].stats[right_Col].count)/n; 
        T[original_right].stats[right_Col].count=T[original_left].stats[left_Col].count;

        T[original_left].stats[left_Col].distinct_count=(T[original_left].stats[left_Col].distinct_count *T[original_right].stats[right_Col].distinct_count)/n; 
        T[original_right].stats[right_Col].distinct_count=T[original_left].stats[left_Col].distinct_count ;


        for(int i=0;i<T[original_left].numColumns;i++){
            if(left_Col==i){
                continue;
            }
            T[original_left].stats[i].count =T[original_left].stats[left_Col].count;
            int temp_num=T[original_left].stats[i].distinct_count*(1-pow(1-T[original_left].stats[left_Col].count/old_left_count,T[original_left].stats[i].count/T[original_left].stats[i].distinct_count)) ;
            T[original_left].stats[i].distinct_count=temp_num;
        }

        for(int i=0;i<T[original_right].numColumns;i++){
            if(right_Col==i){
                continue;
            }
            T[original_right].stats[i].count =T[original_right].stats[right_Col].count;
            int temp_num=T[original_right].stats[i].distinct_count*(1-pow(1-T[original_right].stats[right_Col].count/old_right_count,T[original_right].stats[i].count/T[original_right].stats[i].distinct_count)) ;
            T[original_right].stats[i].distinct_count=temp_num;
        }

    }

    if(right_rel==left_rel){
        int n=T[original_left].stats[left_Col].max-T[original_left].stats[left_Col].min +1;
        T[original_left].stats[left_Col].count=(T[original_left].stats[left_Col].count*T[original_right].stats[right_Col].count)/n;
        for(int i=0;i<T[original_left].numColumns;i++){
            if(left_Col==i){
                continue;
            }
            T[original_left].stats[i].count =T[original_left].stats[left_Col].count;
        }
    
    
    }
    return 0;
}

