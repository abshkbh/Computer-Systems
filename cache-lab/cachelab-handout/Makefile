CC=gcc
LDFLAGS=-lm
CFLAGS=-g -Wall -Wextra -Werror -std=c99 -pedantic

# The student does not make the executable cachesim.example
# This given binary is used by the test to evaluate trans.c

all: cachesim cachesim-debug test test-debug tracegen

cachesim: cachesim.o cachelab-tools.o handin
	$(CC) $(CFLAGS) cachesim.o cachelab-tools.o $(LDFLAGS) -o cachesim

cachesim-debug: cachesim-debug.o cachelab-tools.o handin
	$(CC) $(CFLAGS) cachesim-debug.o cachelab-tools.o $(LDFLAGS) -o cachesim-debug

# The student is not given the test source code
# Instead, they are given a test.o
test.o: 
	cp test-handout test.o

tracegen.o:
	cp tracegen-handout tracegen.o

test: test.o cachelab-tools.o trans.o
	$(CC) $(CFLAGS) -O0 test.o cachelab-tools.o trans.o $(LDFLAGS) -o test

test-debug: test.o cachelab-tools.o trans-debug.o
	$(CC) $(CFLAGS) -O0 test.o cachelab-tools.o trans-debug.o $(LDFLAGS) -o test-debug

tracegen: tracegen.o trans.o cachelab-tools.o
	$(CC) $(CFLAGS) -O0 tracegen.o trans.o cachelab-tools.o $(LDFLAGS) -o tracegen

cachesim.o: cachesim.c
	$(CC) $(CFLAGS) -c cachesim.c

cachesim-debug.o: cachesim.c
	$(CC) $(CFLAGS) -DDEBUG -c cachesim.c -o cachesim-debug.o

cachelab-tools.o: cachelab-tools.c  cachelab-tools.h
	$(CC) $(CFLAGS) -c cachelab-tools.c 

trans.o: trans.c
	$(CC) $(CFLAGS) -O0 -c trans.c

trans-debug.o: trans.c
	$(CC) $(CFLAGS) -DDEBUG -O0 -c trans.c -o trans-debug.o

handin:
	tar -cvf ${USER}_handin.tar  cachesim.c trans.c 

clean:
	rm -rf *.o
	rm -f cachesim
	rm -f test
	rm -f tracegen
