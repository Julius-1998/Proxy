class HttpRequest {
public:
	enum METHOD { GET, POST, CONNECT };
	virtual HttpResponse handle(const Socket& proxy_server_socket) = 0;
};

class GetRequest : public HttpRequest {
	@Override
	virtual HttpResponse handle(const Socket& proxy_server_socket);
};

class PostRequest : public HttpRequest {
	@Override
	virtual HttpResponse handle(const Socket& proxy_server_socket);
};

class ConnnectRequest : public HttpRequest {
	@Override
	virtual HttpResponse handle(const Socket& proxy_server_socket);
};