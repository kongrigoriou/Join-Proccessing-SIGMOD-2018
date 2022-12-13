MY_PATH := $(dir $(lastword $(MAKEFILE_LIST)))
MODULES := $(MY_PATH)source
HEADERS := $(MY_PATH)headers
TESTS	:= $(MY_PATH)tests
LIB		:= $(MY_PATH)lib
OBJ		:= $(MY_PATH)objec
BUILD	:= $(MY_PATH)build

OBJS	= $(OBJ)/bitmap.o $(OBJ)/functions.o $(OBJ)/hashedjoinmain.o $(OBJ)/hopscotch.o $(OBJ)/list.o $(OBJ)/partition.o $(OBJ)/partitionedHashJoin.o $(OBJ)/utilities.o $(OBJ)/queries.o
SOURCE	= $(MODULES)/bitmap.c $(MODULES)/functions.c $(MODULES)/hashedjoinmain.c $(MODULES)/hopscotch.c $(MODULES)/list.c $(MODULES)/partition.c $(MODULES)/partitionedHashJoin.c $(MODULES)/utilities.c $(MODULES)/queries.c
HEADER	= $(HEADERS)/bitmap.h $(HEADERS)/functions.h $(HEADERS)/list.h $(HEADERS)/mainPartitionTest.h $(HEADERS)/structures.h $(HEADERS)/hopscotch.h $(HEADERS).queries.h
OUT	= exec
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lm

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(BUILD)/$(OUT) $(LFLAGS)

hop_test: $(OBJ)/hopscotch.o $(OBJ)/list.o $(OBJ)/bitmap.o $(OBJ)/hopscotch_test.o
	$(CC) -g $(OBJ)/hopscotch.o $(OBJ)/list.o $(OBJ)/bitmap.o $(OBJ)/hopscotch_test.o -o hop_test $(LFLAGS)

partition_test: $(OBJ)/partition.o $(OBJ)/partition_test.o
	$(CC) -g $(OBJ)/partition.o $(OBJ)/partition_test.o -o partition_test $(LFLAGS)

utilities_test: $(OBJ)/utilities.o $(OBJ)/utilities_test.o
	$(CC) -g $(OBJ)/utilities.o $(OBJ)/utilities_test.o -o utilities_test $(LFLAGS)

$(OBJ)/partition_test.o: $(TESTS)/partition_test.c
	$(CC) $(FLAGS) $(TESTS)/partition_test.c -o $(OBJ)/partition_test.o

$(OBJ)/hopscotch_test.o: $(TESTS)/hopscotch_test.c
	$(CC) $(FLAGS) $(TESTS)/hopscotch_test.c -o $(OBJ)/hopscotch_test.o

$(OBJ)/utilities_test.o: $(TESTS)/utilities_test.c
	$(CC) $(FLAGS) $(TESTS)/utilities_test.c -o $(OBJ)/utilities_test.o

$(OBJ)/bitmap.o: $(MODULES)/bitmap.c
	$(CC) $(FLAGS) $(MODULES)/bitmap.c -o $(OBJ)/bitmap.o

$(OBJ)/functions.o: $(MODULES)/functions.c
	$(CC) $(FLAGS) $(MODULES)/functions.c -o $(OBJ)/functions.o

$(OBJ)/hashedjoinmain.o: hashedjoinmain.c
	$(CC) $(FLAGS) hashedjoinmain.c -o $(OBJ)/hashedjoinmain.o

$(OBJ)/hopscotch.o: $(MODULES)/hopscotch.c
	$(CC) $(FLAGS) $(MODULES)/hopscotch.c -o $(OBJ)/hopscotch.o

$(OBJ)/list.o: $(MODULES)/list.c
	$(CC) $(FLAGS) $(MODULES)/list.c -o $(OBJ)/list.o

$(OBJ)/partition.o: $(MODULES)/partition.c
	$(CC) $(FLAGS) $(MODULES)/partition.c -o $(OBJ)/partition.o

$(OBJ)/partitionedHashJoin.o: $(MODULES)/partitionedHashJoin.c
	$(CC) $(FLAGS) $(MODULES)/partitionedHashJoin.c -o $(OBJ)/partitionedHashJoin.o

$(OBJ)/utilities.o: $(MODULES)/utilities.c
	$(CC) $(FLAGS) $(MODULES)/utilities.c -o $(OBJ)/utilities.o

$(OBJ)/queries.o: $(MODULES)/queries.c
	$(CC) $(FLAGS) $(MODULES)/queries.c -o $(OBJ)/queries.o

clean:
	rm -f $(BUILD)/$(OBJS) $(OUT) partition_test hop_test utilities_test partition_test.o hopscotch_test.o utilities_test.o

run: $(BUILD)/$(OUT)
	cat input/small/small.init input/small/small.work | $(BUILD)/$(OUT)

tests: hop_test partition_test utilities_test
	./hop_test
	./partition_test
	./utilities_test

valgrind_join:
	valgrind --leak-check=full --show-leak-kinds=all ./exec ./input/default.txt

valgrind_tests: hop_test partition_test utilities_test
	valgrind --leak-check=full --show-leak-kinds=all ./hop_test
	valgrind --leak-check=full --show-leak-kinds=all ./partition_test
	valgrind --leak-check=full --show-leak-kinds=all ./utilities_test