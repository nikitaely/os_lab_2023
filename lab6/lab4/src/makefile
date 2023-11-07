CC=gcc
CFLAGS=-I.
.PHONY : all sequential_min_max parallel_min_max clean process_memory parallel_sum

all : parallel_min_max process_memory parallel_sum


#sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
#	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

parallel_sum: utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_sum utils.o find_min_max.o parallel_sum.c -lpthread $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

process_memory :
	$(CC) $(CFLAGS)  process_memory.c -o process_memory 

clean :
	rm -f utils.o find_min_max.o sequential_min_max parallel_min_max process_memory parallel_sum
