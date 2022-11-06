#include "structures.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "bitmap.h"
#include "list.h"
#include "hopscotch.h"



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
uint32_t hash2(uint32_t x){
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

//makes sure your hash fits the array
int hash(int key,int size_of_array){
    return hash2(key)%size_of_array;
}

//gets H
//makes sure search in H costs O(logn)
int get_H(hop* array){
    return array->H;
}

node* create_array_of_hop(int n, int h){
    
    node* array=malloc(n*sizeof(node));
    int bitmap_size=sizeof(int);
    int H=h;
    if(bitmap_size<H){
        bitmap_size=H;
    }
    for(int i=0;i<n;i++){
        array[i].bitmap=create_bitmap(bitmap_size);
        *(array[i].bitmap)=(char)0;
        array[i].occupied=0;
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
    
    printf("end of creation \n");
    return hops;
}


void destroy_array(node* array, int size){
    printf("start of destroy array\n");
    
    for(int i =0;i<size;i++){
        free(array[i].bitmap);
    }
    free(array);
    printf("end of destroy array\n");
}

void destroy_hop(hop* hops){
    
    int size =hops->size;
    printf("before destroy_array size =%d\n",hops->size);
    destroy_array(hops->array,size);
    printf("after destroy_array\n");
    free(hops);    
}


// if bitmap is full
int bitmapisfull(bitmap_t bitmap, int size){
    int isfull=1;
    for(int i=0;i<size;i++){
        int bit=get_bit((bitmap_t)bitmap,i);
        printf("Bit=%d\n",bit);
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
            printf("\nindex: %d key=%d payload=%d bitmap:\n",i,(array[i].info).key,(array[i].info).payload);
            print_bitmap(array[i].bitmap);
            printf("\n\n");
        }
    }
}

int resize(hop** hops_, tuple element, int n, int h){
    
    hop* hops=*hops_;
    node* current_array=hops->array;
    
    node* new_array=create_array_of_hop(2*n,2*h);
    hops->array=new_array;
    hops->H=2*h;
    hops->size=2*n;
    //node* new_array=hops->array;
    for(int i=0;i<n;i++){
        if(current_array[i].occupied!=0){
            insert(hops,current_array[i].info);
        }
    }
    destroy_array(current_array,n);

    int new_size=insert(hops,element);
    
    
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
int insert(hop* hops, tuple element){
    printf("\n\nlet's insert this %d\n\n",element.key);
    node* array=hops->array;
    int n= hops->size;
    int H =get_H(hops);
    int key=element.key;
    printf("\n\nsize of array is %d\n\n",n);
    int hash_of_key=hash(key,n);
    printf("\n\nhash of key is %d\n\n",hash_of_key);
    bitmap_t bitmap=array[hash_of_key].bitmap;
    int size;
    printf("\n\nH is %d in this insert and key=%d\n\n",H,element.key);
    // if a neighborhood is full of elements with hash(element) we need a bigger array
    if(bitmapisfull(bitmap,H)){
        print_bitmap(bitmap);
        printf("\nYou should appear here at least once\n");
        size=resize(&hops,element,n,H);
        //resize
        return size;
    }
    

    int j=closest_empty_spot(array,hash_of_key,n);
    printf("\n\nJ is %d in this insert\n\n",j);
    //if there is no empty spot array is full
    if(j==-1){
        size=resize(&hops,element,n,H);
        //resize
        return size;
    }
    while(dist(j,hash_of_key,n)>=H){
        
        int check_index=j-H+1;
        int element_to_be_moved=-1;
        for(int i=0,j=H-1;i<H-1;i++,j--,check_index++){
            if(check_index<0){
                check_index=check_index+n;
            }
            element_to_be_moved= get_first_1((array[check_index].bitmap ),j)+check_index;
            
            if(element_to_be_moved>=n){
                element_to_be_moved=element_to_be_moved%n;
            }

            if(element_to_be_moved!=-1){
                break;
            }
        }
        if(element_to_be_moved==-1){
            size=resize(&hops,element,n,H);
            //resize
            
            return size;
        }
        clear_bit((bitmap_t)(array[check_index].bitmap ),element_to_be_moved);
        array[element_to_be_moved].occupied=0;
        array[j].occupied=1;
        array[j].info=array[element_to_be_moved].info;
        set_bit((bitmap_t)array[j].bitmap,0);
        j=check_index;
    }
    array[j].info=element;
    printf("\nset bit with index %d\n",abs(j-hash_of_key));
    set_bit((bitmap_t)(array[hash_of_key].bitmap),dist(j,hash_of_key,n));
    array[j].occupied=1;
    print_array(array,n);
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
    List * result_list=list_create();
    for(int i=hash_of_key, bit_i=0;bit_i<H;i++,bit_i++){
        if(i>=size){
            i=0;
        }
        printf("hello\n");
        int curr_bit=get_bit((bitmap_t)bitmap,bit_i);
        if(curr_bit==1&&(array[i].info).key==element.key){
            /*result[bit_i].occupied=1;
            result[bit_i].index=element.payload;
            */
           tuple temp_t;
           temp_t.key=(array[i].info).payload;
           temp_t.payload=element.payload;
           list_insert(result_list,temp_t);
        }
        
        
    }
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