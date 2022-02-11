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
		if (fd == -1) {
			// TODO
		}
		listenfd = fd;
	}

	SocketBuilder(const char* port) : SocketBuilder(const_cast<char*>(port)) {}

	SocketBuilder(const std::string& port) : SocketBuilder(port.c_str()) {}

	Socket openTCPConnection(const std::string& host, const std::string& port) {
		int fd = open_clientfd(const_cast<char*>(host.c_str()), const_cast<char*>(port.c_str()));
		if (fd == -1) {
			// TODO
		}
		return Socket(fd);
	}
	Socket acceptTCPConnection() {
		int connfd = accept(listenfd, (SA*)&clientaddr, &clientlen);
		if (connfd == -1) {
			// TODO
		}
		return Socket(connfd);
	}
};

#endif
