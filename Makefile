MY_PATH := $(dir $(lastword $(MAKEFILE_LIST)))
MODULES := $(MY_PATH)source
HEADERS := $(MY_PATH)headers
TESTS	:= $(MY_PATH)tests
LIB		:= $(MY_PATH)lib
OBJ		:= $(MY_PATH)objec
BUILD	:= $(MY_PATH)build

OBJS	= $(OBJ)/bitmap.o $(OBJ)/functions.o $(OBJ)/hashedjoinmain.o $(OBJ)/hopscotch.o $(OBJ)/list.o $(OBJ)/partition.o $(OBJ)/partitionedHashJoin.o $(OBJ)/utilities.o $(OBJ)/queries.o
SOURCE	= $(MODULES)/bitmap.c $(MODULES)/functions.c $(MODULES)/hashedjoinmain.c $(MODULES)/hopscotch.c $(MODULES)/list.c $(MODULES)/partition.c $(MODULES)/partitionedHashJoin.c $(MODULES)/utilities.c $(MODULES)/queries.c harness.cpp
HEADER	= $(HEADERS)/bitmap.h $(HEADERS)/functions.h $(HEADERS)/list.h $(HEADERS)/mainPartitionTest.h $(HEADERS)/structures.h $(HEADERS)/hopscotch.h $(HEADERS).queries.h
OUT	= exec
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lm

all: $(OBJS)
	@ mkdir -p ./build
	$(CC) -g $(OBJS) -o $(BUILD)/$(OUT) $(LFLAGS)
	g++ -o $(OBJ)/harness harness.cpp
	chmod +x runTestharness.sh
	chmod +x run.sh


hop_test: $(OBJ)/hopscotch.o $(OBJ)/list.o $(OBJ)/bitmap.o $(OBJ)/hopscotch_test.o
	@ mkdir -p ./$(BUILD)
	$(CC) -g $(OBJ)/hopscotch.o $(OBJ)/list.o $(OBJ)/bitmap.o $(OBJ)/hopscotch_test.o -o $(BUILD)/hop_test $(LFLAGS)

partition_test: $(OBJ)/partition.o $(OBJ)/partition_test.o
	$(CC) -g $(OBJ)/partition.o $(OBJ)/partition_test.o -o $(BUILD)/partition_test $(LFLAGS)

utilities_test: $(OBJ)/utilities.o $(OBJ)/utilities_test.o
	$(CC) -g $(OBJ)/utilities.o $(OBJ)/utilities_test.o -o $(BUILD)/utilities_test $(LFLAGS)
	
list_test: $(OBJ)/list.o $(OBJ)/list_test.o
	$(CC) -g $(OBJ)/list.o $(OBJ)/list_test.o -o $(BUILD)/list_test $(LFLAGS)

	
$(OBJ)/partition_test.o: $(TESTS)/partition_test.c
	$(CC) $(FLAGS) $(TESTS)/partition_test.c -o $(OBJ)/partition_test.o

$(OBJ)/hopscotch_test.o: $(TESTS)/hopscotch_test.c
	$(CC) $(FLAGS) $(TESTS)/hopscotch_test.c -o $(OBJ)/hopscotch_test.o

$(OBJ)/utilities_test.o: $(TESTS)/utilities_test.c
	$(CC) $(FLAGS) $(TESTS)/utilities_test.c -o $(OBJ)/utilities_test.o
	
$(OBJ)/list_test.o: $(TESTS)/list_test.c
	$(CC) $(FLAGS) $(TESTS)/list_test.c -o $(OBJ)/list_test.o

# $(OBJ)/harness.o: harness.cpp
# 	@ mkdir -p ./objec

$(OBJ)/bitmap.o: $(MODULES)/bitmap.c
	@ mkdir -p ./objec
	$(CC) $(FLAGS) $(MODULES)/bitmap.c -o $(OBJ)/bitmap.o

$(OBJ)/functions.o: $(MODULES)/functions.c
	$(CC) $(FLAGS) $(MODULES)/functions.c -o $(OBJ)/functions.o

$(OBJ)/hashedjoinmain.o: hashedjoinmain.c
	$(CC) $(FLAGS) hashedjoinmain.c -o $(OBJ)/hashedjoinmain.o

$(OBJ)/hopscotch.o: $(MODULES)/hopscotch.c
	@ mkdir -p ./objec
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
	rm -f $(OBJS) $(BUILD)/$(OUT) $(BUILD)/partition_test $(BUILD)/hop_test $(BUILD)/utilities_test $(BUILD)/list_test $(OBJ)/partition_test.o $(OBJ)/hopscotch_test.o $(OBJ)/utilities_test.o $(OBJ)/list_test.o

run: $(BUILD)/$(OUT)
	cat input/default.txt| $(BUILD)/$(OUT)

tests: hop_test partition_test utilities_test list_test
	./$(BUILD)/hop_test
	./$(BUILD)/partition_test
	./$(BUILD)/utilities_test
	./$(BUILD)/list_test

valgrind_join:
	cat input/default.txt| valgrind --leak-check=full --show-leak-kinds=all $(BUILD)/$(OUT)

valgrind_tests: hop_test partition_test utilities_test list_test
	valgrind --leak-check=full --show-leak-kinds=all ./$(BUILD)/hop_test
	valgrind --leak-check=full --show-leak-kinds=all ./$(BUILD)/partition_test
	valgrind --leak-check=full --show-leak-kinds=all ./$(BUILD)/utilities_test
	valgrind --leak-check=full --show-leak-kinds=all ./$(BUILD)/list_test