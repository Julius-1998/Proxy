CC = g++
CFLAGS = -std=c++11 -Wall

all: proxy

csapp.o: csapp.c csapp.h
	gcc -Wall -c csapp.c

BlockingQueue.o: BlockingQueue.cpp
	$(CC) $(CFLAGS) -c BlockingQueue.cpp

proxy.o: proxy.c csapp.h
	$(CC) $(CFLAGS) -c proxy.c

proxy: proxy.o socks.o csapp.o
	$(CC) $(CFLAGS) proxy.o socks.o csapp.o -o proxy $(LDFLAGS)

clean:
	rm -f *~ *.o proxy
