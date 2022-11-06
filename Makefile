OBJS	= bitmap.o functions.o hashedjoinmain.o hopscotch.o list.o partition.o partitionedHashJoin.o utilities.o
SOURCE	= bitmap.c functions.c hashedjoinmain.c hopscotch.c list.c partition.c partitionedHashJoin.c utilities.c
HEADER	= bitmap.h functions.h list.h mainPartitionTest.h structures.h hopscotch.h
OUT	= exec
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lm

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

bitmap.o: bitmap.c
	$(CC) $(FLAGS) bitmap.c 

functions.o: functions.c
	$(CC) $(FLAGS) functions.c 

hashedjoinmain.o: hashedjoinmain.c
	$(CC) $(FLAGS) hashedjoinmain.c 

hopscotch.o: hopscotch.c
	$(CC) $(FLAGS) hopscotch.c 

list.o: list.c
	$(CC) $(FLAGS) list.c 

partition.o: partition.c
	$(CC) $(FLAGS) partition.c 

partitionedHashJoin.o: partitionedHashJoin.c
	$(CC) $(FLAGS) partitionedHashJoin.c 

utilities.o: utilities.c
	$(CC) $(FLAGS) utilities.c 


clean:
	rm -f $(OBJS) $(OUT)

run: $(OUT)
	./$(OUT) inputSmall.txt