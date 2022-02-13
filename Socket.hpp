#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

extern "C" {
    #include "csapp.h"
}

class Socket {
private:
	int fd = -1;
    std::string readLine() {
        char buf[1];
        std::string content;
        while (true) {
            read(fd, buf, 1);
            if (*buf == '\r') {
                read(fd, buf, 1);
                break;
            }
            content.push_back(*buf);
        }
        return content;
    }
public:
	Socket(int fd) : fd(fd) {}

	Socket(Socket&& that) {
		this->fd = that.fd;
		that.fd = -1;
	}

	Socket(Socket& that) = delete;
	Socket(const Socket& that) = delete;

	HttpRequest recvRequest() {
        // TODO
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
