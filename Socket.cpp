#include "csapp.h"
#include <tuple>
#include <optional>
class Socket {
private:
	int fd;
public:
	Socket(int fd) : fd(fd) {}

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
		close(fd);
	}

};