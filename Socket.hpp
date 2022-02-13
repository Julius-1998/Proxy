#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#define MAX_LINE (1 << 12)

extern "C" {
    #include "csapp.h"
}

class Socket {
private:
	int fd = -1;
    rio_t rio;

    std::string readLine() {
        char buf[MAX_LINE];
        rio_readlineb(&rio, buf, MAX_LINE);
        return std::string(buf);
    }
public:
	Socket(int fd) : fd(fd) {
        rio_readinitb(&rio, fd);
    }

	Socket(Socket&& that) {
		this->fd = that.fd;
		that.fd = -1;
	}

	Socket(Socket& that) = delete;
	Socket(const Socket& that) = delete;

	HttpRequest recvRequest() {
        // TODO
        std::vector<char> raw_data;
        std::string start_line = readLine();

        return ConnectRequest();
	}

	HttpResponse recvResponse() {
        // TODO
        return HttpResponse();
	}

	void sendRequest(const HttpRequest& request) {

	}

	void sendResponse(const HttpResponse& response) {

	}

	int getFileDescriptor() {
		return fd;
	}
	
	~Socket() {
		if (fd >= 0)
			close(fd);
	}

};

#endif
