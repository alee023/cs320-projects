CC=gcc

test: test.o
	$(CC) test.o -o test

test.o: test.c
	$(CC) -c test.c -o test.o

clean:
	rm -f test.o