#ifndef __SOCKETBUILDER_HPP__
#define __SOCKETBUILDER_HPP__

#include <string>

class SocketBuilder {
private:
	int listenfd;
	socklen_t clientlen;
    struct sockaddr_storage clientaddr;
public:
	SocketBuilder(char* port) {
		int fd = open_listenfd(port);
		listenfd = fd;
	}

	SocketBuilder(const char* port) : SocketBuilder(const_cast<char*>(port)) {}

	SocketBuilder(const std::string& port) : SocketBuilder(port.c_str()) {}

	Socket openTCPConnection(const std::string& host, const std::string& port) {
		int fd = open_clientfd(const_cast<char*>(host.c_str()), const_cast<char*>(port.c_str()));
		return Socket(fd);
	}
	Socket acceptTCPConnection(int unique_id) {
        
		int connfd = accept(listenfd, (SA*)&clientaddr, &clientlen);
        // fprintf(stderr, "Request from %s\n", inet_ntoa(((struct sockaddr_in *)&clientaddr)->sin_addr));
		return Socket(connfd, unique_id);
	}
};

#endif
