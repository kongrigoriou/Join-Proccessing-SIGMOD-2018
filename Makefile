CC = gcc
CFLAGS = -Wall

hopscotch: hopscotch.o bitmap.o 
	$(CC) -o hopscotch bitmap.o hopscotch.o $(CFLAGS) -lm 

hopscotch.o:
	$(CC) hopscotch.c -c $(CFLAGS)


bitmap.o: bitmap.c
	$(CC) bitmap.c -c $(CFLAGS)

clean:
	rm -f *.o