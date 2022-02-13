#ifndef __HTTPREQUEST_HPP__
#define __HTTPREQUEST_HPP__
#include <string>
#include <vector>
class Socket;
class HttpRequest {
protected:
    HttpRequest() { }
    std::string host;
    std::string port;
    std::vector<char> raw_data;
public:
	enum METHOD { GET, POST, CONNECT };
    METHOD method;
    HttpRequest(const std::string& host, const std::string& port) : host(host), port(port) {}
	virtual HttpResponse handle(const Socket& out) { return HttpResponse(); }; 
    void setStatus(METHOD method) { this->method = method; }
    std::string getHost() { return host; }
    std::string getPort() { return port; }
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
