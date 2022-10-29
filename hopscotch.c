#include "structures.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "bitmap.h"



// node of hopscotch array
typedef struct node {
    struct tuple info;  //the tuple
    uint32_t  bitmap;   //the bitmap
    int occupied;        //whether node is empty or not
} node;

int insert(node* new_array,tuple element,int size);

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

//creates the array
node* create_array(int n){
    
    return malloc(n*sizeof(node));
}

//gets H
//makes sure search in H costs O(logn)
int get_H(int n){
    return (int)ceil(log(n));
}

// if bitmap is full
int bitmapisfull(uint32_t bitmap, int size){
    int isfull=1;
    for(int i=0;i<size;i++){
        int bit=get_bit((bitmap_t)&bitmap,i);
        printf("Bit=%d\n",bit);
        if(bit==0){
            isfull=0;
            break;
        }
    }
    return isfull;
}

//return position of first 1 in bitmap
int get_first_1(uint32_t bitmap, int size){
    
    for(int i=0;i<size;i++){
        int bit=get_bit((bitmap_t)&bitmap,i);
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
    printf("\nH=%d\n",get_H(size));
    for(int i=0;i<size;i++){
        if(array[i].occupied==1){   //if node has an element
            printf("\nindex: %d key=%d payload=%d bitmap:\n",i,(array[i].info).key,(array[i].info).payload);
            print_bitmap(array[i].bitmap);
            printf("\n\n");
        }
    }
}

int resize(node** array, tuple element, int n){
    node* new_array=create_array(2*n);
    node * current_array=*array;
    for(int i=0;i<n;i++){
        if(current_array[i].occupied!=0){
            insert(new_array,current_array[i].info,2*n);
        }
    }
    free(*array);
    int new_size=insert(new_array,element,2*n);
    array=&new_array;
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
int insert(node* array, tuple element, int n){
    int H =get_H(n);
    int key=element.key;
    int hash_of_key=hash(key,n);
    uint32_t bitmap=array[hash_of_key].bitmap;
    int size;
    printf("\n\nH is %d in this insert and key=%d\n\n",H,element.key);
    // if a neighborhood is full of elements with hash(element) we need a bigger array
    if(bitmapisfull(bitmap,H)){
        print_bitmap(bitmap);
        printf("\nYou should appear here at least once\n");
        size=resize(&array,element,n);
        //resize
        return size;
    }
    

    int j=closest_empty_spot(array,hash_of_key,n);
    printf("\n\nJ is %d in this insert\n\n",j);
    //if there is no empty spot array is full
    if(j==-1){
        size=resize(&array,element,n);
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
            size=resize(&array,element,n);
            //resize
            
            return size;
        }
        clear_bit((bitmap_t)&(array[check_index].bitmap ),element_to_be_moved);
        array[element_to_be_moved].occupied=0;
        array[j].occupied=1;
        array[j].info=array[element_to_be_moved].info;
        set_bit((bitmap_t)&array[j].bitmap,0);
        j=check_index;
    }
    array[j].info=element;
    printf("\nset bit with index %d\n",abs(j-hash_of_key));
    set_bit((bitmap_t)&(array[hash_of_key].bitmap),dist(j,hash_of_key,n));
    array[j].occupied=1;
    print_array(array,n);
    return n;
}

typedef struct result_node {
    int index;          //the tuple
    int occupied;       //whether node is empty or not
} result_node;

result_node* search(node* array,tuple element, int size){
    int hash_of_key=hash( element.key,size);
    uint32_t bitmap=array[hash_of_key].bitmap;
    int H=get_H(size);
    result_node* result=malloc(sizeof(tuple)*H);
    for(int i=hash_of_key, bit_i=0;bit_i<H;i++,bit_i++){
        if(i>=size){
            i=0;
        }
        printf("hello\n");
        int curr_bit=get_bit((bitmap_t)&bitmap,bit_i);
        if(curr_bit==1&&(array[i].info).key==element.key){
            result[bit_i].occupied=1;
            result[bit_i].index=element.payload;
        }
        else{
            result[bit_i].occupied=0;
        }
        
    }
    return result;
}

int main(){
    node* array=create_array(10);

    tuple t;
    t.key=0;
    t.payload=0;
    int size=10;
    for(int i=0;i<2;i++){

        t.key=9;
        size=insert(array,t,size);
    }
    t.key=1;
    size=insert(array,t,size);
    t.key=1;
    size=insert(array,t,size);
    t.key=0;
    size=insert(array,t,size);
    print_array(array,size);
    t.key=9;
    result_node* r;
    r=search(array,t, size);
    for(int i=0;i<get_H(size);i++){
        if(r[i].occupied==1){
            printf("\n%d:%d\n",i,r[i].index);
        }
    }
    
    
}