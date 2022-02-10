class SocketBuilder {
	static Socket openTCPConn(char* host, char* port);
	static Socket acceptConn(int listenfd);
};

class Task {  // redundant?
	Socket client_proxy_socket;

	void execute() {
		client_proxy_socket.read().handle().close();
	}


};


class HttpResponse {
public:
	
};

class HttpRequest {
public:
	static int GET, POST, CONNECT;
	virtual tuple<HttpResponse, optional<Socket>> handle() = 0;
};

class GetRequest : public HttpRequest {
	@Override
	virtual tuple<HttpResponse, optional<Socket>> handle() = 0;
};

class PostRequest : public HttpRequest {
	@Override
	virtual tuple<HttpResponse, optional<Socket>> handle() = 0;
};

class ConnnectRequest : public HttpRequest {
	@Override
	virtual tuple<HttpResponse, optional<Socket>> handle() = 0;
};

class Socket {
private:
	int fd;
public:
	HttpRequest read();
	void write(HttpResponse response);
	int getFileDescriptor();
	int close();

};

class BlockingQueue<T> {
	queue<T> q;
	mutex lock;
};

ThreadPool thread_pool {
	ThreadPool(int num, function thread_func);
	void start();
};

class BlindForwarder {
private:
	struct pollfd pfds[2] = {0};
public:
	BlindForwarder(Socket in, Socket out) {
    	pfds[0].fd = in.getFileDescriptor();
    	pfds[1].fd = out.getFileDescriptor();
    	pfds[0].events = pfds[1].events = POLLIN;
	}

	void forward() {
		char buf[MAX_READ];
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
	            cnt2 = recv(pfds[1], buf, MAX_READ, 0);
	            send(pfds[0], buf, cnt2, 0);
	        }
	        if (errno == ECONNRESET) break;
	    }
	}



};

int main(int argc, char const *argv[]) {
	BlockingQueue<Socket> socket_pool;
	ThreadPool thread_pool(16, thread_func);
	thread_pool.start();
	while (1) {
        Socket socket = SocketBuilder.acceptConn(listenfd);
        socket_pool.push(socket);
	}
	return 0;
}



void thread_func() {
	while (1) {
		Socket client_proxy_socket = socket_pool.pop();
		HttpRequest request = client_proxy_socket.read();
		HttpResponse response;
		optional<Socket> optional_socket;
		tie(response, optional_socket) = request.handle();
		client_proxy_socket.write(response);

		// OPTION 1
		if (request.type == HttpResponse::CONNECT) {
			BlindForwarder forwarder(client_proxy_socket, proxy_server_socket);
			forwarder.forward();
		}
		// OPTION 2
		if (optional_socket.hasValue()) {
			Socket proxy_server_socket = optional_socket.value();
			BlindForwarder forwarder(client_proxy_socket, proxy_server_socket);
			forwarder.forward();
			proxy_server_socket.close();
		}
		client_proxy_socket.close();
    }
}

void 








