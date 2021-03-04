CC	= gcc
FLAGS   = -g -c -Wall

all: prog

prog: ArgUtil.o Hash.o LinkedList.o StringUtil.o Trace.o Hashes.o main.o
	$(CC) -g ArgUtil.o Hash.o LinkedList.o StringUtil.o Trace.o Hashes.o main.o -lm -o prog

main.o: main.c
	$(CC) $(FLAGS) main.c

Hashes.o: Hashes.c
	$(CC) $(FLAGS) Hashes.c

ArgUtil.o: ArgUtil.c
	$(CC) $(FLAGS) ArgUtil.c

Hash.o: Hash.c
	$(CC) $(FLAGS) Hash.c

LinkedList.o: LinkedList.c
	$(CC) $(FLAGS) LinkedList.c

StringUtil.o: StringUtil.c
	$(CC) $(FLAGS) StringUtil.c

Trace.o: Trace.c
	$(CC) $(FLAGS) Trace.c

clean:
	rm -f *.o prog
