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
    array->capacity = 55;

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

QueryArray* get_batch(){
    char c;
    int query_num = 0;
    QueryArray* batch = array_create();

    fflush(stdout);
    scanf("%c", &c);
    // printf("initial c %c\t", c);
    if(c != '1' && c != '2' && c != '3' &&c != '4' &&c != '5' &&c != '6' &&c != '7' &&c != '8' &&c != '9' &&c != '0'){
        // printf("\tinseide null\n");
        return NULL;
    }

    while(c != 'F'){

        int i = 0;
        int i_filters = 0;
        char int_buffer[5];
        int rel_id = 0;
        int int_buffer_count = 0;

        while(c != '|'){
            while(c != ' ' && c != '|'){
                int_buffer[int_buffer_count] = c;     
                int_buffer_count++;

                scanf("%c", &c);
            }
            
            for(int j = 0; j < int_buffer_count; j++){
                rel_id *= 10;
                rel_id += (int_buffer[j] - 48);
            }
            batch->array[query_num]->relationsId[i] = rel_id;
            rel_id = 0;
            
            int_buffer_count = 0;
            i++;

            if(c != '|')
                scanf("%c", &c);
        }

        batch->array[query_num]->relationsCount = i;
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
                            batch->array[query_num]->filters[i_filters] = buffer[j];
                            i_filters++;
                        }
                    }
                    batch->array[query_num]->filtersCount = i_filters;
                }else{
                    for(int j = 0; j < buffcount; j++){
                        if(buffer[j] != '.'){
                            batch->array[query_num]->predicates[i] = buffer[j];
                            i++;
                        }
                    }
                    batch->array[query_num]->predicatesCount = i;
                    // printf("Predicates count: %d\n", batch->array[query_num]->predicatesCount);
                }
                // printf("BATCHES: %d\n", batch_num);
                // printf("QUERY: %d\n", query_num);
            }else{
                for(int j = 0; j < buffcount; j++){
                    if(buffer[j] != '.'){
                        batch->array[query_num]->filters[i_filters] = buffer[j];
                        i_filters++;
                    }
                }
                buffcount = 0;
                batch->array[query_num]->filtersCount = i_filters;
                // printf("filter count: %d\n", batch->array[query_num]->filtersCount);
            }

            buffcount = 0;
            
            // break;
            if(c != '|')
                scanf("%c", &c);
        }
        //to take the sums 
        scanf("%c", &c);

        i = 0;    
        while(c != '\n'){
            if(c != '.' && c != ' '){
                batch->array[query_num]->selections[i] = c;
                i++;
            }
            scanf("%c", &c);
        }
        batch->array[query_num]->selectionsCount = i;
        query_num++;

        scanf("%c", &c);
        // printf("the last %c\n", c);
    }

    batch->size = query_num;
    return batch;
}
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
        if(c == 'F'){
            batch_num++;
            batches->size = batch_num;
            query_num = 0;
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
            while(c != ' ' && c != '|'){
                int_buffer[int_buffer_count] = c;     
                int_buffer_count++;

                scanf("%c", &c);
            }
            
            for(int j = 0; j < int_buffer_count; j++){
                rel_id *= 10;
                rel_id += (int_buffer[j] - 48);
            }
            batches->batches[batch_num]->array[query_num]->relationsId[i] = rel_id;
            rel_id = 0;
            
            int_buffer_count = 0;
            i++;

            if(c != '|')
                scanf("%c", &c);
        }

        batches->batches[batch_num]->array[query_num]->relationsCount = i;
        scanf("%c", &c);
        
        i = 0;
        char buffer[10];
        int buffcount = 0;
    
        while(c != '|'){
            printf("c inside while %c\n", c);
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
                // printf("filter count: %d\n", batches->batches[batch_num]->array[query_num]->filtersCount);
            }

            buffcount = 0;
            
            // break;
            if(c != '|')
                scanf("%c", &c);
        }

        //to take the sums 
        scanf("%c", &c);

        i = 0;    
        while(c != '\n'){
            if(c != '.' && c != ' '){
                batches->batches[batch_num]->array[query_num]->selections[i] = c;
                i++;
            }
            scanf("%c", &c);
        }
        batches->batches[batch_num]->array[query_num]->selectionsCount = i;
        query_num++;
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

void print_batch(QueryArray* batch){

    printf("Batch Size: %d", batch->size);
    printf("Relations\n");
    for(int i = 0; i < batch->size; i++){
        for(int j = 0; j < batch->array[i]->relationsCount; j++){
            printf("%d ", batch->array[i]->relationsId[j]);
        }
    }
    printf("\n");

    printf("fiters\n");
    for(int i = 0; i < batch->size; i++){
        for(int j = 0; j < batch->array[i]->filtersCount; j++){
            printf("%c ", batch->array[i]->filters[j]);
        }
    }
    printf("\n");

    printf("predicates\n");
    for(int i = 0; i < batch->size; i++){
        // printf("Predicates count %d\n", batch->array[i]->predicatesCount);

        for(int j = 0; j < batch->array[i]->predicatesCount; j++){
            printf("%c ", batch->array[i]->predicates[j]);
        }
    }
    printf("\n");

    printf("sums\n");
    for(int i = 0; i < batch->size; i++){
        for(int j = 0; j < batch->array[i]->selectionsCount; j++){
            printf("%c ", batch->array[i]->selections[j]);
        }
    }
    printf("\n");
}

// int main(void){
//     Batches* query;
//     query = get_query_info();
//     print(query);

//     destroy(query);
// }
