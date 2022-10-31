#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

typedef unsigned char* bitmap_t;

void set_bit(bitmap_t b, int i) {
    b[i / 8] |= 1 << (i & 7);
}

void clear_bit(bitmap_t b, int i) {
    b[i / 8] &= ~(1 << (i & 7));
}

int get_bit(bitmap_t b, int i) {
    return b[i / 8] & (1 << (i & 7)) ? 1 : 0;
}

void print_bitmap(bitmap_t array){
    printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(*array));
}

bitmap_t create_bitmap(int n) {
    return malloc((n + 7) / 8);
}


void bitmap_main(){
    uint32_t array;
    array=0;
    set_bit((unsigned char*)&array,0);
    set_bit((unsigned char*)&array,1);
    printf("%u should be 3\n",array);
}