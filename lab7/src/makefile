CC=gcc
CFLAGS=-I.
.PHONY : tcpclient tcpserver udpclient udpserver

all : tcpclient tcpserver udpclient udpserver

#sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
#   $(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

tcpclient : 
    $(CC) -o tcpclient tcpclient.c $(CFLAGS)
tcpserver : 
    $(CC) -o tcpserver tcpserver.c $(CFLAGS)
udpclient : 
    $(CC) -o udpclient udpclient.c $(CFLAGS)
udpserver : 
    $(CC) -o udpserver udpserver
    .c $(CFLAGS)

clean :
    rm -f tcpclient tcpserver udpclient udpserver

