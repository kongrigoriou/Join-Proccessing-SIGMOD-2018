#ifndef BITMAP_FILE
#define BITMAP_FILE

typedef unsigned char* bitmap_t;

int get_bit(bitmap_t* array, int index);
void set_bit(bitmap_t* array, int index);
void clear_bit(bitmap_t* array, int index);
void print_bitmap(uint32_t array);

#endif