#ifndef __HTTPREQUEST_HPP__
#define __HTTPREQUEST_HPP__

class HttpRequest
{
protected:
    std::string host;
    std::string port;
    std::string url;
    std::vector<char> raw_data;
    std::unordered_map<std::string, std::string> header_fields;
    int unique_id;

public:
    enum METHOD
    {
        GET,
        POST,
        CONNECT
    };
    METHOD method;
    HttpRequest(int unique_id) : unique_id(unique_id) {}
    void setMethod(METHOD method) { this->method = method; }
    int getUniqueId() const { return unique_id; }
    std::string getHost() const { return host; }
    std::string getPort() const { return port; }
    std::string getUrl() const { return url; }
    std::string getCacheKey() const { return getHost() + ":" + getPort() + getUrl(); }
    std::string getField(const std::string &field) const
    {
        if (header_fields.count(field))
            return header_fields.at(field);
        return "";
    }
    std::string getMethodString() const
    {
        if (method == GET)
        {
            return "GET";
        }
        else if (method == POST)
        {
            return "POST";
        }
        else if (method == CONNECT)
        {
            return "CONNECT";
        }
        //todo: throw an error here
        return "Invalid";
    }
    std::string getRequestLine() const { return getMethodString() + getHost() + ":" + getPort() + getUrl(); }
    const std::vector<char> &getRawData() { return raw_data; }
    void appendRawData(const std::string &data)
    {
        for (char c : data)
            raw_data.push_back(c);
    }

    void appendRawData(const char *data)
    {
        while (*data)
            raw_data.push_back(*data++);
    }
    void appendRawData(const char *data, size_t size)
    {
        while (size--)
            raw_data.push_back(*data++);
    }
    void setField(const std::string &k, const std::string &v) { header_fields[k] = v; }
    void setHost(const std::string &host) { this->host = host; }
    void setPort(const std::string &port) { this->port = port; }
    void setUrl(const std::string &url) { this->url = url; }
    METHOD getMethod() { return method; }
    std::string isCachable() const
    {
        if (getField("no-store") != "")
            return "";
        return "request specifies no-cache";
    }
    virtual ~HttpRequest() {}
};
/*
class GetRequest : public HttpRequest {
public:
    GetRequest() { setMethod(GET); }
    virtual HttpResponse handle(const Socket& out) override {
       // out.sendRawData(getRawData().data(), getRawData().size());
       // return out.recvResponse();
       return HttpResponse();
    }
};

class PostRequest : public HttpRequest {
public:
    PostRequest() { setMethod(POST); }

    virtual HttpResponse handle(const Socket& out) override {
        // out.sendRawData(getRawData().data(), getRawData().size());
        // return out.recvResponse();
        return HttpResponse();
    }
};

class ConnectRequest : public HttpRequest {
public:
    ConnectRequest() { setMethod(CONNECT); }
    virtual HttpResponse handle(const Socket& out) override {
        // out.sendRawData(getRawData().data(), getRawData().size());
        //TODO
        // Do we have to produce response ourself?
        // return out.recvResponse();
        return HttpResponse();
    }
};
*/

class HttpRequestWrapper
{
private:
    HttpRequest *request;

public:
    HttpRequestWrapper(int unique_id)
    {
        request = new HttpRequest(unique_id);
    }
    /*
    HttpRequestWrapper(const std::string& method, int unique_id) {

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
    }*/

    HttpRequestWrapper &operator=(HttpRequestWrapper &&that)
    {
        request = that.request;
        that.request = nullptr;
        return *this;
    }

    HttpRequestWrapper(HttpRequestWrapper &&that)
    {
        request = that.request;
        that.request = nullptr;
    }
    HttpRequestWrapper(HttpRequestWrapper &that) = delete;
    HttpRequestWrapper(const HttpRequestWrapper &that) = delete;

    HttpRequest *get()
    {
        return request;
    }
    /*
    HttpResponse handle(const Socket& out) {
        return request->handle(out);
    }
    */
    int getUniqueId() const { return request->getUniqueId(); }
    std::string getHost() const { return request->getHost(); }
    std::string getPort() const { return request->getPort(); }
    std::string getUrl() const { return request->getUrl(); }
    std::string getCacheKey() const { return request->getCacheKey(); }
    std::string getField(const std::string &field) const { return request->getField(field); }
    const std::vector<char> &getRawData() const { return request->getRawData(); }
    void appendRawData(const std::string &data) { request->appendRawData(data); }

    void appendRawData(const char *data) { request->appendRawData(data); }
    void appendRawData(const char *data, size_t size) { request->appendRawData(data, size); }
    void setField(const std::string &k, std::string v) { request->setField(k, v); }
    void setHost(const std::string &host) { request->setHost(host); }
    void setPort(const std::string &port) { request->setPort(port); }
    void setUrl(const std::string &url) { request->setUrl(url); }
    HttpRequest::METHOD getMethod() { return request->getMethod(); }

    std::string isCachable() const { return request->isCachable(); }
    ~HttpRequestWrapper()
    {
        delete request;
    }
};

#endif
