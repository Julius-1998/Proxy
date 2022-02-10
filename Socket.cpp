#include "csapp.h"
#include <tuple>
#include <optional>
class Socket {
private:
	int fd;
public:
	Socket(int fd) : fd(fd) {}

	HttpRequest read() {

	}
	void write(HttpResponse response) {

	}

	int getFileDescriptor() {
		return fd;
	}
	
	~Socket() {
		close(fd);
	}

};