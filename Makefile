CC = gcc
CFLAGS = -Wall

hopscotch: hopscotch.o bitmap.o list.o
	$(CC) -o ./build/hopscotch bitmap.o hopscotch.o list.o $(CFLAGS) -lm 

hopscotch.o:
	$(CC) hopscotch.c -c $(CFLAGS)

list.o:
	$(CC) list.c -c $(CFLAGS)

bitmap.o: bitmap.c
	$(CC) bitmap.c -c $(CFLAGS)

clean:
	rm -f *.o