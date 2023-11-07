CC=gcc
CFLAGS=-I.
.PHONY : mutex fact

all : mutex fact


#sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
#	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

fact : 
	$(CC) -o fact fact.c -lpthread $(CFLAGS)

mutex : 
	$(CC) -o mutex mutex.c -lpthread $(CFLAGS)


clean :
	rm -f mutex fact
