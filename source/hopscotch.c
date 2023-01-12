#include "../headers/structures.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include "../headers/bitmap.h"
#include "../headers/list.h"
#include "../headers/hopscotch.h"

// node of hopscotch array
// typedef struct node {
//     struct tuple info;  //the tuple
//     bitmap_t  bitmap;   //the bitmap
//     int occupied;        //whether node is empty or not
    
// } node;

// typedef struct hopscotch_array {
//     node* array;
//     int H;
//     int size;
// } hop;

// int insert(hop* new_array,tuple element);

//found in https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
uint64_t hash2(uint64_t x){
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

//makes sure your hash fits the array
int hash(int key,int size_of_array){
    return hash2(key)%size_of_array;//hash2(key)
}

//gets H
//makes sure search in H costs O(logn)
int get_H(hop* array){
    return array->H;
}

node* create_array_of_hop(int n, int h){
    
    node* array=malloc(n*sizeof(node));
    int bitmap_size=h;//sizeof(int);
    int H=h;
    if(bitmap_size<H){
        bitmap_size=H;
    }
    for(int i=0;i<n;i++){
        array[i].info.key=0;
        array[i].info.payload=0;
        array[i].bitmap=create_bitmap(bitmap_size);
        //*(array[i].bitmap)=(char)0;
        array[i].occupied=0;
        //array[i].duplicate=list_create();
        //array[i].overflow=list_create();
    }
    return array;
}

//creates the array
hop* create_array(int n, int h){
    //node * array=malloc(n*sizeof(node));
    printf("let's create a hopscotch struct\n");
    hop* hops=malloc(sizeof(hop));
    
    hops->H=h;
    hops->size=n;
    hops->array=create_array_of_hop(n,h);
    

    //printf("end of creation \n");
    return hops;
}


void destroy_array(node* array, int size){
    // printf("start of destroy array\n");
    
    for(int i =0;i<size;i++){
        free(array[i].bitmap);
    }
    free(array);
    // printf("end of destroy array\n");
}

void destroy_hop(hop* hops){
    
    int size =hops->size;
    // printf("before destroy_array size =%d\n",hops->size);
    destroy_array(hops->array,size);
    // printf("after destroy_array\n");
    free(hops);    
}


// if bitmap is full
int bitmapisfull(bitmap_t bitmap, int size){
    int isfull=1;
    for(int i=0;i<size;i++){
        int bit=get_bit((bitmap_t)bitmap,i);
        // printf("Bit=%d\n",bit);
        if(bit==0){
            isfull=0;
            break;
        }
    }
    return isfull;
}

//return position of first 1 in bitmap
int get_first_1(bitmap_t bitmap, int size){
    
    for(int i=0;i<size;i++){
        int bit=get_bit((bitmap_t)bitmap,i);
        if(bit==1){
            return i;
        }
    }
    return -1;
}

//find closest empty space
//if there are non, return -1
int closest_empty_spot(node* array,int start, int size){
    for(int count=0,i=start;count<size;count++){
        if(array[i].occupied==0){
            return i;
        }
        i++;
        if(i==size){
            i=0;
        }
    }
    return -1;
}

//print hopscotch array used mainly for debugginh
void print_array(node* array, int size){
    //printf("\nH=%d\n",get_H(size));
    for(int i=0;i<size;i++){
        if(array[i].occupied==1){   //if node has an element
            // printf("\nindex: %d key=%d payload=%d bitmap:\n",i,(array[i].info).key,(array[i].info).payload);
            print_bitmap(array[i].bitmap);
            // printf("\n\n");
        }
    }
}
int blah=1;
int resize(hop** hops_, tuple element, int n, int h){
    
    hop* hops=*hops_;
    node* current_array=hops->array;
    printf("RESIZE\n");
    if(h<10000&&blah==1){
        hops->H=2*h;//log((double)hops->size);
        //blah=0;
    }
    else{
        hops->H=h+h/2;
        blah=1;
    }
    if(n<1000000){
        hops->size=2*n;
    }
    else{
        hops->size=n+n/2;
    }
    node* new_array=create_array_of_hop(hops->size,hops->H);
    hops->array=new_array;
    //int size=hops->size;
    
    //node* new_array=hops->array;
    for(int i=0;i<n;i++){
        if(current_array[i].occupied!=0){
            insert(hops,current_array[i].info, NULL, NULL, NULL);    //›˜ÔıÏÂ Í‹ÌÂÈ ﬁ‰Á lock ÙÔ write mutex
        }
        //ListNode* node;
        //node = current_array[i].overflow->head;
        /*for(int j=0;j<current_array[i].overflow->size;j++){
            printf("Inserting things\n");
            insert(hops,node->data);
            node = node->next;
        }*/
    }
    destroy_array(current_array,n);

    int new_size=insert(hops,element, NULL, NULL, NULL);    //����� ����� ��� lock �� write mutex
   

    //*hops_=hops;
    //printf("hops=%d and old_hops=%d\n\n",hops,old_hops);
    
    return new_size;
}

int dist(int j, int hash,int n){
    for(int i=0,step=hash;i<n;i++,step++){
        if(step>=n){
            step=0;
        }
        if(step==j){
            return i;
        }
    }
    //should never reach this point
    return -1;
}

// inserts the element into hopscotch array with H size of neighberhood and n size of array
//retruns the size of the array after the insert
int insert(hop* hops, tuple element, pthread_mutex_t* hopMutexRead, pthread_mutex_t* hopMutexWrite, int* noOfReaders){

    
    if (hopMutexRead != NULL) {
        pthread_mutex_lock(hopMutexRead);
        (*noOfReaders)++;
        if ((*noOfReaders) == 1)
            pthread_mutex_lock(hopMutexWrite);
        pthread_mutex_unlock(hopMutexRead);
    }
    node* array=hops->array;
    uint64_t n= hops->size;
    uint64_t H =get_H(hops);
    int key=element.key;
    //printf("\n\nsize of array is %d\n\n",n);
    int hash_of_key=hash(key,n);
    //printf("\n\nhash of key is %d\n\n",hash_of_key);
    bitmap_t bitmap=array[hash_of_key].bitmap;
    int size;
    /*if(element.payload==16646){
        printf("\n\nH is %d in this insert and key=%ld\n\n",H,element.key);
    }*/
    /*for(int i=0;i<H;i++){
        if(get_bit(bitmap,i)){
            if(array[hash_of_key+i].info.key==element.key){
                list_insert(array[hash_of_key+i].duplicate,element);
                return n;
            }
        }
    }*/
    int bitmapIsFull = bitmapisfull(bitmap, H);
    if (hopMutexRead != NULL) {
        pthread_mutex_lock(hopMutexRead);
        (*noOfReaders)--;
        if ((*noOfReaders) == 0)
            pthread_mutex_unlock(hopMutexWrite);
        pthread_mutex_unlock(hopMutexRead);
    }
    // if a neighborhood is full of elements with hash(element) we need a bigger array
    if(bitmapIsFull){
        if (hopMutexRead != NULL)
            pthread_mutex_lock(hopMutexWrite);
        //print_bitmap(bitmap);
        //printf("\nYou should appear here at least once\n");
        size =n;
        /*if(hash_of_key==hash(key,2*n)){
            printf("things are entering the overflow list hash_of_key=%d\n",hash_of_key);
            list_insert( array[hash_of_key].overflow,element);
        }
        else{
            size=resize(&hops,element,n,H);
        }*/
        //resize
        size=resize(&hops,element,n,H);
        if (hopMutexRead != NULL)
            pthread_mutex_unlock(hopMutexWrite);
        return size;
    }
    if (hopMutexRead != NULL) {
        pthread_mutex_lock(hopMutexRead);
        (*noOfReaders)++;
        if ((*noOfReaders) == 1)
            pthread_mutex_lock(hopMutexWrite);
        pthread_mutex_unlock(hopMutexRead);
    }
    int j=closest_empty_spot(array,hash_of_key,n);
    //printf("\n\nJ is %d in this insert\n\n",j);
    /*if(element.payload==16646){
        printf("\n\nJ is %d in this insert\n\n",j);
    }*/
    if (hopMutexRead != NULL) {
        pthread_mutex_lock(hopMutexRead);
        (*noOfReaders)--;
        if ((*noOfReaders) == 0)
            pthread_mutex_unlock(hopMutexWrite);
        pthread_mutex_unlock(hopMutexRead);
    }
    //if there is no empty spot array is full
    if(j==-1){
        if (hopMutexRead != NULL)
            pthread_mutex_lock(hopMutexWrite);
        size =n;
        /*if(hash_of_key==hash(key,2*n)){
            printf("things are entering the overflow list h=%d\n",hash_of_key);
            list_insert( array[hash_of_key].overflow,element);
            printf("things are entering the overflow list h=%d overflow size=%d\n",hash_of_key,array[hash_of_key].overflow->size);
        }
        else{
            size=resize(&hops,element,n,H);
        }*/
        //resize
        size=resize(&hops,element,n,H);
        if (hopMutexRead != NULL)
            pthread_mutex_unlock(hopMutexWrite);
        return size;
    }
    while(dist(j,hash_of_key,n)>=H){//dist(j,hash_of_key,n)
        
        int check_index=j-H+1;
        int element_to_be_moved=-1;
        int pos_in_bitmap=-1;
        for(int i=0, k=H-1;i<H-1;i++,k--,check_index++){
            if(check_index<0){
                check_index=check_index+n;
            }
            if(check_index>=n){
                check_index=check_index%n;
            }
            //printf("before pos in bitmap %d\n",check_index);
            pos_in_bitmap=get_first_1((array[check_index].bitmap ),k);
            
            element_to_be_moved= pos_in_bitmap+check_index;
            
            if(element_to_be_moved>=n){
                element_to_be_moved=element_to_be_moved%n;
            }

            if(pos_in_bitmap!=-1){
                break;
            }
        }
        if(pos_in_bitmap==-1){
            if (hopMutexRead != NULL)
                pthread_mutex_lock(hopMutexWrite);
            size =n;
            /*if(hash_of_key==hash(key,2*n)){
                list_insert( array[hash_of_key].overflow,element);
            }
            else{
                size=resize(&hops,element,n,H);
            }*/

            //resize
            size=resize(&hops,element,n,H);
            if (hopMutexRead != NULL)

            return size;
        }
            if (hopMutexRead != NULL)
                pthread_mutex_unlock(hopMutexWrite);
            printf("\ni am to be moved here j=%d, before i was here %d and hash=%d and H=%d check=%d\n",j, element_to_be_moved, hash(array[element_to_be_moved].info.key,n),H,check_index);
        }*/
        //printf("pos of bitmap is=%d\n",pos_in_bitmap);
        if (hopMutexRead != NULL)
            pthread_mutex_lock(hopMutexWrite);
        clear_bit(array[check_index].bitmap ,pos_in_bitmap);
        //printf("where\n");
        array[element_to_be_moved].occupied=0;
        //printf("still\n");
        array[j].occupied=1;
        array[j].info=array[element_to_be_moved].info;
        //printf("at least here dist=%d\n",dist(element_to_be_moved,hash_of_key,n));
        set_bit(array[check_index].bitmap,dist(j,check_index,n));
        if (hopMutexRead != NULL)
            pthread_mutex_unlock(hopMutexWrite);
        //printf("after set bit\n");
        j=element_to_be_moved;
    }
    //printf("before if\n");
    if (hopMutexRead != NULL)
        pthread_mutex_lock(hopMutexWrite);
    array[j].info=element;
    //printf("\nset bit with index %d\n",abs(j-hash_of_key));
    set_bit((array[hash_of_key].bitmap),dist(j,hash_of_key,n));
    array[j].occupied=1;
    if (hopMutexRead != NULL)
        pthread_mutex_unlock(hopMutexWrite);
    //print_array(array,n);
    //printf("end of insert\n");
    return n;
}

// typedef struct result_node {
//     int index;          //the tuple
//     int occupied;       //whether node is empty or not
// } result_node;

List* search(hop*  hops ,tuple element){
    node* array=hops->array;
    int size=hops->size;
    int hash_of_key=hash( element.key,size);
    bitmap_t bitmap=array[hash_of_key].bitmap;
    int H=get_H(hops);

    //result_node* result=malloc(sizeof(tuple)*H);
    //printf("Search me\n");
    List * result_list=list_create();
    for(int i=hash_of_key, bit_i=0;bit_i<H;i++,bit_i++){
        if(i>=size){
            i=0;
        }
       
        
        int curr_bit=get_bit(bitmap,bit_i);
        //printf("after get bit\n");
        if(curr_bit==1&&(array[i].info).key==element.key&&array[i].occupied==1){//curr_bit==1&&
            /*result[bit_i].occupied=1;
            result[bit_i].index=element.payload;
            */
            
           tuple temp_t;
           temp_t.key=(array[i].info).payload;
           temp_t.payload=element.payload;

           list_insert(result_list,temp_t);
           //result_list=list_append(result_list,array[i].duplicate);
        }
        //printf("after if\n");
        
    }
    //ListNode* node;
    /*node = array[hash_of_key].overflow->head;
    for(int i=0;i<array[hash_of_key].overflow->size;i++){
        printf("In the overflow list\n");
        if(node->data.key==element.key){
            printf("Insert in result_list\n");
            tuple temp_t;
            temp_t.key=node->data.payload;
            temp_t.payload=element.payload;
            list_insert(result_list,temp_t);
        }
        node = node->next;
    }*/
    //list_print(result_list);
    return result_list;
}

void print_hops(hop* hops){
    print_array(hops->array,hops->size);
}

// int main(){
//     hop* array=create_array(10,1);

//     tuple t;
//     t.key=0;
//     t.payload=0;
//     int size=10;
//     for(int i=0;i<4;i++){

//         t.key=9;
//         size=insert(array,t);
//     }
//     printf("after loop\n");
//     print_hops(array);

    
//     //return 0;
//     t.key=1;
//     size=insert(array,t);
//     t.key=1;
//     size=insert(array,t);
//     t.key=0;
//     size=insert(array,t);
//     print_hops(array);
//     t.key=9;
//     List* r;
//     r=search(array,t);
//     list_print(r);
//     list_destroy(r); 
//     destroy_hop(array);
    
// }
