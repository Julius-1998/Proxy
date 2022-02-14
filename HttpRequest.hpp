#ifndef __HTTPREQUEST_HPP__
#define __HTTPREQUEST_HPP__
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
extern "C" {
    #include "csapp.h"
}
class Socket;
class HttpRequest {
protected:
    std::string host;
    std::string port;
    std::string url;
    std::vector<char> raw_data;
    std::unordered_map<std::string, std::string> header_fields;
public:
	enum METHOD { GET, POST, CONNECT };
    METHOD method;
	virtual HttpResponse handle(const Socket& out) = 0; 
    void setMethod(METHOD method) { this->method = method; }
    std::string getHost() { return host; }
    std::string getPort() { return port; }
    std::string getField(const std::string& field) {
        if (header_fields.count(field))
            return header_fields[field];
        return "";
    }
    std::vector<char>& getRawData() { return raw_data; }
    void appendRawData(const std::string& data) { for (char c : data) raw_data.push_back(c); }
    void setField(const std::string& k, const std::string& v) { header_fields[k] = v; }
    void setHost(const std::string& host) { this->host = host; }
    void setPort(const std::string& port) { this->port = port; }
    void setUrl(const std::string& url) { this->url = url; }
    METHOD getMethod() { return method; }

    virtual ~HttpRequest() {}

};

class GetRequest : public HttpRequest {
public:
    GetRequest() { setMethod(GET); }
	virtual HttpResponse handle(const Socket& out) override {
        return HttpResponse();
    }
    

    
};

class PostRequest : public HttpRequest {
public:
    PostRequest() { setMethod(POST); }

	virtual HttpResponse handle(const Socket& out) override {
        return HttpResponse();
    }
};

class ConnectRequest : public HttpRequest {
public:
    ConnectRequest() { setMethod(CONNECT); }
	virtual HttpResponse handle(const Socket& out) override {
        return HttpResponse();
    }
};


class HttpRequestWrapper {
private:
    HttpRequest* request;
public:
    HttpRequestWrapper(const std::string& method) {
        if (method == "GET") {
            request = new GetRequest();
        } else if (method == "POST") {
            request = new PostRequest();
        } else if (method == "CONNECT") {
            request = new ConnectRequest();
        } else {
            //TODO not implemented error
            request = nullptr;
        }
    }

    HttpRequestWrapper& operator=(HttpRequestWrapper&& that) {
        request = that.request;
        that.request = nullptr;
        return *this;
    }

    HttpRequestWrapper(HttpRequestWrapper&& that) {
        request = that.request;
        that.request = nullptr;
    }
    HttpRequestWrapper(HttpRequestWrapper& that) = delete; 
    HttpRequestWrapper(const HttpRequestWrapper& that) = delete;
    
    HttpRequest* get() {
       return request; 
    }
    HttpResponse handle(const Socket& out) {
        return request->handle(out);
    } 
    std::string getHost() { return request->getHost(); }
    std::string getPort() { return request->getPort(); }
    std::string getField(const std::string& field) { return request->getField(field); }
    std::vector<char>& getRawData() { return request->getRawData(); }
    void appendRawData(const std::string& data) { request->appendRawData(data); }
    void setField(const std::string& k, std::string v) { request->setField(k, v); }
    void setHost(const std::string& host) { request->setHost(host); }
    void setPort(const std::string& port) { request->setPort(port); }
    void setUrl(const std::string& url) { request->setUrl(url); }
    HttpRequest::METHOD getMethod() { return request->getMethod(); }
    ~HttpRequestWrapper() {
        delete request;
    }

};

#endif
