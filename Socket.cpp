#include "csapp.h"
#include <tuple>
#include <optional>
class Socket {
private:
	int fd = -1;
public:
	Socket(int fd) : fd(fd) {}

	Socket(Socket&& that) {
		this->fd = that->fd;
		that->fd = -1;
	}

	Socket(Socket that) = delete;
	Socket(Socket& that) = delete;
	Socket(const Socket& that) = delete;

	HttpRequest recvRequest() {

	}

	HttpResponse recvResponse() {

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