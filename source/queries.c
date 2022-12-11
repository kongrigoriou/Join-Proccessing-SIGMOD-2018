#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/queries.h"

QueryInfo* info_create(){
    
    QueryInfo* queryInfo;

    queryInfo = malloc(sizeof(QueryInfo));
    queryInfo->relationsCount = 0;
    queryInfo->filtersCount = 0;
    queryInfo->predicatesCount = 0;
    queryInfo->selectionsCount = 0;

    queryInfo->relationsId = malloc(4 * sizeof(int));
    queryInfo->predicates = malloc(150 * sizeof(char));
    queryInfo->filters = malloc(150 * sizeof(char));
    queryInfo->selections = malloc(150 * sizeof(char));

    return queryInfo;
}

QueryArray* array_create(){
    QueryArray* array;

    array = malloc(sizeof(QueryArray));
    array->capacity = 20;

    array->array = malloc(array->capacity * sizeof(QueryInfo*));

    for(int i = 0; i < array->capacity; i++){
        array->array[i] = info_create();
    }

    array->size = 0;

    return array;
}

Batches* batches_create(){
    Batches* batches;

    batches = malloc(sizeof(Batches));
    batches->batches = malloc(10 * sizeof(QueryArray*));

    for(int i = 0; i < 10; i++){
        batches->batches[i] = array_create();
    }

    batches->size = 0;
    batches->capacity = 10;

    return batches;
}

void destroy(Batches* array){
    for(int i = 0; i < array->capacity; i++){
        for(int j = 0; j < array->batches[i]->capacity; j++){
            free(array->batches[i]->array[j]->relationsId);
            free(array->batches[i]->array[j]->filters);
            free(array->batches[i]->array[j]->selections);
            free(array->batches[i]->array[j]->predicates);

            free(array->batches[i]->array[j]);
        }
        free(array->batches[i]->array);
        free(array->batches[i]);
    }
    free(array->batches);
    free(array);
}

// void resize(void* array){

// }

Batches* get_query_info(){

    Batches* batches;
    batches = batches_create();

    int batch_num = 0;
    int query_num = 0;

    char c;
    int i = 0;
    int i_filters = 0;
    int endOfFile = 0;
    while(endOfFile == 0){

        scanf("%c", &c);
        printf("\t\tC: %c\n", c);
        if(c == 'F'){
            batch_num++;
            batches->size = batch_num;
            query_num = 0;
            // printf("Batch size: %d\n", batches->size);
        }

        if(c == 'F'){
            scanf("%c", &c);
            scanf("%c", &c);
        
            if(c == '\n'){
                endOfFile = 1;
                break;
            }
        }
        i = 0;
        i_filters = 0;
        char int_buffer[5];
        int rel_id = 0;
        int int_buffer_count = 0;

        while(c != '|'){

            // printf("c: %c\n", c);
            while(c != ' ' && c != '|'){
                // batches->batches[batch_num]->array[query_num]->relationsId[i] = atoi(&c);
                // printf("inside if\n\tc is: %c\n", c);
                int_buffer[int_buffer_count] = c;     
                int_buffer_count++;

                scanf("%c", &c);
            }
            
            for(int j = 0; j < int_buffer_count; j++){
                rel_id *= 10;
                rel_id += (int_buffer[j] - 48);
            }
            printf("rel id: %d\n", rel_id);
            batches->batches[batch_num]->array[query_num]->relationsId[i] = rel_id;
            rel_id = 0;
            
            int_buffer_count = 0;
            i++;

            if(c != '|')
                scanf("%c", &c);
        }

        batches->batches[batch_num]->array[query_num]->relationsCount = i;
        printf("relations count: %d\n", batches->batches[batch_num]->array[query_num]->relationsCount);

        scanf("%c", &c);
        
        i = 0;
        char buffer[10];
        int buffcount = 0;
    
        while(c != '|'){
            // printf("c inside while %c\n", c);
            while(c != '&' && c!= '|'){
                buffer[buffcount] = c;
                buffcount++;

                scanf("%c", &c);
            }

            buffer[buffcount] = '|';
            buffcount++;

            if(buffer[buffcount -3] == '.'){
                if(buffer[0] == buffer[4]){
                    //then its a filter
                    for(int j = 0; j < buffcount; j++){
                        if(buffer[j] != '.'){
                            batches->batches[batch_num]->array[query_num]->filters[i_filters] = buffer[j];
                            i_filters++;
                        }
                    }
                    batches->batches[batch_num]->array[query_num]->filtersCount = i_filters;
                }else{
                    for(int j = 0; j < buffcount; j++){
                        if(buffer[j] != '.'){
                            batches->batches[batch_num]->array[query_num]->predicates[i] = buffer[j];
                            i++;
                        }
                    }
                    batches->batches[batch_num]->array[query_num]->predicatesCount = i;
                }
                // printf("BATCHES: %d\n", batch_num);
                // printf("QUERY: %d\n", query_num);
            }else{
                for(int j = 0; j < buffcount; j++){
                    if(buffer[j] != '.'){
                        batches->batches[batch_num]->array[query_num]->filters[i_filters] = buffer[j];
                        i_filters++;
                    }
                }
                buffcount = 0;
                batches->batches[batch_num]->array[query_num]->filtersCount = i_filters;
                printf("filter count: %d\n", batches->batches[batch_num]->array[query_num]->filtersCount);
            }

            buffcount = 0;
            
            // break;
            if(c != '|')
                scanf("%c", &c);

            // printf("\tC1: %c\n", c);
        }

        //to take the sums 
        scanf("%c", &c);

        i = 0;    
        while(c != '\n'){
            // printf("\tC2: %c\n", c);

            if(c != '.' && c != ' '){
                batches->batches[batch_num]->array[query_num]->selections[i] = c;
                i++;
            }

            scanf("%c", &c);

        }
        batches->batches[batch_num]->array[query_num]->selectionsCount = i;
        query_num++;
        // printf("\tQUERY NUM: %d\n", query_num);
        batches->batches[batch_num]->size = query_num;
    }

    return batches;
}

void print_queries(Batches* batches){

    printf("Relations\n");
    for(int i = 0; i < batches->size; i++){
        for(int j = 0; j < batches->batches[i]->size; j++){
            for(int k = 0; k < batches->batches[i]->array[j]->relationsCount; k++){
                printf("%d ", batches->batches[i]->array[j]->relationsId[k]);
            }
            printf("\n");
        }
    }

    printf("predicates\n");
    for(int i = 0; i < batches->size; i++){
        for(int j = 0; j < batches->batches[i]->size; j++){
            for(int k = 0; k < batches->batches[i]->array[j]->predicatesCount; k++){
                printf("%c", batches->batches[i]->array[j]->predicates[k]);
            }
            printf("\n");
        }
    }

    printf("Filters\n");
    for(int i = 0; i < batches->size; i++){
        for(int j = 0; j < batches->batches[i]->size; j++){
            for(int k = 0; k < batches->batches[i]->array[j]->filtersCount; k++){
                printf("%c", batches->batches[i]->array[j]->filters[k]);
            }
            printf("\n");
        }
    }

    printf("Sums\n");
    for(int i = 0; i < batches->size; i++){
        for(int j = 0; j < batches->batches[i]->size; j++){
            for(int k = 0; k < batches->batches[i]->array[j]->selectionsCount; k++){
                printf("%c ", batches->batches[i]->array[j]->selections[k]);
            }
            printf("\n");
        }
    }
}

// int main(void){
//     Batches* query;
//     query = get_query_info();
//     print(query);

//     destroy(query);
// }
