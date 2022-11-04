OBJS	= hopscotch.o partition.o bitmap.o partitionedHashJoin.o list.o utilities.o functions.o
SOURCE	= hopscotch.c partition.c bitmap.c partitionedHashJoin.c list.cutilities.c functions.c
HEADER	= hopscotch.h bitmap.h mainPartitionTest.h list.h functions.h
OUT	= test
CC	 = gcc
FLAGS	 = -g3 -c -Wall
LFLAGS	 = -lm

all: $(OBJS)
	$(CC) -g3 $(OBJS) -o $(OUT) $(LFLAGS)

bitmap.o: bitmap.c
	$(CC) $(FLAGS) bitmap.c 

hopscotch.o: hopscotch.c
	$(CC) $(FLAGS) hopscotch.c 

# hopscotch_test.o: hopscotch_test.c
# 	$(CC) $(FLAGS) hopscotch_test.c 

partition.o: partition.c
	$(CC) $(FLAGS) partition.c

partitionedHashJoin.o: partitionedHashJoin.c
	$(CC) $(FLAGS) partitionedHashJoin.c

list.o: list.c
	$(CC) $(FLAGS) list.c

utilities.o: utilities.c
	$(CC) $(FLAGS) utilities.c 

mainPartitionTest.o: mainPartitionTest.c
	$(CC) $(FLAGS) mainPartitionTest.c

functions.o: functions.c
	$(CC) $(FLAGS) functions.c

clean:
	rm -f $(OBJS) $(OUT)

run: $(OUT)
	./$(OUT)