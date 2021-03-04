CC	= gcc
FLAGS   = -g -c -Wall

all: prog

prog: ArgUtil.o Hash.o LinkedList.o StringUtil.o Trace.o Hashes.o main.o
	$(CC) -g ArgUtil.o Hash.o LinkedList.o StringUtil.o Trace.o Hashes.o main.o -lm -o prog

main.o: ./src/main.c
	$(CC) $(FLAGS) ./src/main.c

Hashes.o: ./src/Hashes.c
	$(CC) $(FLAGS) ./src/Hashes.c

ArgUtil.o: ./src/ArgUtil.c
	$(CC) $(FLAGS) ./src/ArgUtil.c

Hash.o: ./src/Hash.c
	$(CC) $(FLAGS) ./src/Hash.c

LinkedList.o: ./src/LinkedList.c
	$(CC) $(FLAGS) ./src/LinkedList.c

StringUtil.o: ./src/StringUtil.c
	$(CC) $(FLAGS) ./src/StringUtil.c

Trace.o: ./src/Trace.c
	$(CC) $(FLAGS) ./src/Trace.c

clean:
	rm -f *.o prog
