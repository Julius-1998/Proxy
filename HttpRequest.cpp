class HttpRequest {
public:
	enum METHOD { GET, POST, CONNECT };
	virtual HttpResponse handle() = 0;
};

class GetRequest : public HttpRequest {
	@Override
	virtual HttpResponse handle();
};

class PostRequest : public HttpRequest {
	@Override
	virtual HttpResponse handle();
};

class ConnnectRequest : public HttpRequest {
	@Override
	virtual HttpResponse handle();
};