#ifndef __HTTPREQUEST_HPP__
#define __HTTPREQUEST_HPP__
#include <string>

class Socket;
class HttpRequest {
protected:
    HttpRequest() { }
public:
	enum METHOD { GET, POST, CONNECT };
    METHOD method;

	virtual HttpResponse handle(const Socket& out) { return HttpResponse(); }; 
    void setStatus(METHOD method) { this->method = method; }
    std::string getHost() { return "localhost"; }
    std::string getPort() { return "80"; }
};

class GetRequest : public HttpRequest {
public:
    GetRequest() { setStatus(GET); }
	virtual HttpResponse handle(const Socket& out) override {
        return HttpResponse();
    }
    

    
};

class PostRequest : public HttpRequest {
public:
    PostRequest() { setStatus(POST); }

	virtual HttpResponse handle(const Socket& out) override {
        return HttpResponse();
    }
};

class ConnectRequest : public HttpRequest {
public:
    ConnectRequest() { setStatus(CONNECT); }
	virtual HttpResponse handle(const Socket& out) override {
        return HttpResponse();
    }
};

#endif
