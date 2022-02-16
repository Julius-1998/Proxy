CC = g++
CFLAGS = -std=c++11 -Wall  -ggdb3

all:
	(make clean; make proxy)
csapp.o: csapp.c
	gcc -Wall -c csapp.c

#proxy.o: proxy.cpp Socket.hpp SocketBuilder.hpp Task.hpp ThreadPool.hpp HttpRequest.hpp HttpResponse.hpp BlockingQueue.hpp BlindForwarder.hpp
#	$(CC) $(CFLAGS) -c proxy.cpp Socket.hpp SocketBuilder.hpp Task.hpp ThreadPool.hpp HttpRequest.hpp HttpResponse.hpp BlockingQueue.hpp BlindForwarder.hpp

proxy.o: proxy.cpp
	$(CC) $(CFLAGS) -c proxy.cpp

proxy: proxy.o csapp.o
	$(CC) $(CFLAGS) proxy.o csapp.o -o proxy -lpthread

clean:
	rm -f *.gch *.o proxy
