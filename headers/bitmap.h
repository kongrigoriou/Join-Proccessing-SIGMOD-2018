#ifndef BITMAP_FILE
#define BITMAP_FILE

typedef unsigned char* bitmap_t;

int get_bit(bitmap_t array, int index);
void set_bit(bitmap_t array, int index);
void clear_bit(bitmap_t array, int index);
void print_bitmap(bitmap_t array);
bitmap_t create_bitmap(int n);
int get_count(bitmap_t b,int size);

#endif