MY_PATH := $(dir $(lastword $(MAKEFILE_LIST)))
MODULES := $(MY_PATH)source
HEADERS := $(MY_PATH)headers
TESTS	:= $(MY_PATH)tests
LIB		:= $(MY_PATH)lib

OBJS	= bitmap.o functions.o hashedjoinmain.o hopscotch.o list.o partition.o partitionedHashJoin.o utilities.o
SOURCE	= $(MODULES)/bitmap.c $(MODULES)/functions.c $(MODULES)/hashedjoinmain.c $(MODULES)/hopscotch.c $(MODULES)/list.c $(MODULES)/partition.c $(MODULES)/partitionedHashJoin.c $(MODULES)/utilities.c
HEADER	= $(HEADERS)/bitmap.h $(HEADERS)/functions.h $(HEADERS)/list.h $(HEADERS)/mainPartitionTest.h $(HEADERS)/structures.h $(HEADERS)/hopscotch.h
OUT	= exec
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lm

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

hop_test: hopscotch.o list.o bitmap.o hopscotch_test.o
	$(CC) -g hopscotch.o list.o bitmap.o hopscotch_test.o -o hop_test $(LFLAGS)

partition_test: partition.o partition_test.o
	$(CC) -g partition.o partition_test.o -o partition_test $(LFLAGS)

partition_test.o: $(TESTS)/partition_test.c
	$(CC) $(FLAGS) $(TESTS)/partition_test.c

hopscotch_test.o: $(TESTS)/hopscotch_test.c
	$(CC) $(FLAGS) $(TESTS)/hopscotch_test.c

bitmap.o: $(MODULES)/bitmap.c
	$(CC) $(FLAGS) $(MODULES)/bitmap.c 

functions.o: $(MODULES)/functions.c
	$(CC) $(FLAGS) $(MODULES)/functions.c 

hashedjoinmain.o: hashedjoinmain.c
	$(CC) $(FLAGS) hashedjoinmain.c 

hopscotch.o: $(MODULES)/hopscotch.c
	$(CC) $(FLAGS) $(MODULES)/hopscotch.c 

list.o: $(MODULES)/list.c
	$(CC) $(FLAGS) $(MODULES)/list.c 

partition.o: $(MODULES)/partition.c
	$(CC) $(FLAGS) $(MODULES)/partition.c 

partitionedHashJoin.o: $(MODULES)/partitionedHashJoin.c
	$(CC) $(FLAGS) $(MODULES)/partitionedHashJoin.c 

utilities.o: $(MODULES)/utilities.c
	$(CC) $(FLAGS) $(MODULES)/utilities.c 


clean:
	rm -f $(OBJS) $(OUT) partition_test hop_test partition_test.o hopscotch_test.o

run: $(OUT)
	./$(OUT) ./input/default.txt