#ifndef __BLINDFORWARDER_HPP__
#define __BLINDFORWARDER_HPP__

#include <poll.h>
#include <sys/socket.h>


// #define MAX_READ (1 << 16) Note: Defined in Socket.hpp

class BlindForwarder {
private:
	Socket in;
	Socket out;
public:
	BlindForwarder(Socket&& in, Socket&& out) : in(std::move(in)), out(std::move(out)) {}

	void forward() {
		char buf[MAX_READ];
		struct pollfd pfds[2] = {0};
		pfds[0].fd = in.getFileDescriptor();
    	pfds[1].fd = out.getFileDescriptor();
    	pfds[0].events = pfds[1].events = POLLIN;
		int cnt1 = -1, cnt2 = -1;
	    while (poll(pfds, 2, -1) != -1) {
	        cnt1 = cnt2 = -1;
	        if (pfds[0].revents & POLLERR) break;
	        if (pfds[1].revents & POLLERR) break;
	        if (pfds[0].revents & POLLHUP) break;
	        if (pfds[1].revents & POLLHUP) break;
	        if (pfds[0].revents & POLLIN) {
	            cnt1 = recv(pfds[0].fd, buf, MAX_READ, 0);
	            if (cnt1 == 0) break;
	            send(pfds[1].fd, buf, cnt1, 0);
	        }
	        if (pfds[1].revents & POLLIN) {
	            cnt2 = recv(pfds[1].fd, buf, MAX_READ, 0);
	            send(pfds[0].fd, buf, cnt2, 0);
	        }
	        if (errno == ECONNRESET) break;
	    }
	}
};

#endif
