#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

extern "C" {
    #include "csapp.h"
}

class Socket {
private:
	int fd = -1;
    std::string readLine() {
        char buf[1] = {0};
        std::string content;
        while (*buf != '\n') {
            read(fd, buf, 1);
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
