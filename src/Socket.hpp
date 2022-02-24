#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#define MAX_LINE (1 << 16)
#define MAX_READ (1 << 16)

#include <algorithm>
#include <sstream>
#include "logger.hpp"
class Socket
{
private:
    int fd = -1;
    rio_t rio;
    int unique_id;
    std::string readLine() {
        char buf[MAX_LINE];
        int cnt = rio_readlineb(&rio, buf, MAX_LINE);
        if (cnt == 0)
            return "";
        if (cnt < 0)
            return "";
        return std::string(buf);
    }

    void parseCacheControl(HttpResponse& response) {
        std::string cc_fields = response.getField("CACHE-CONTROL");
        if (cc_fields == "")
            return;
        char key[128], value[128];
        const int len = cc_fields.size();
        int i = 0;
        while (i < len) {
            std::string token;
            while (i < len && cc_fields[i] != ',')
                token.push_back(cc_fields[i++]);
            key[0] = value[0] = 0;
            sscanf(token.c_str(), "%[^=]=%s", key, value);
            response.setField(std::string(key), strlen(value) ? std::string(value) : "true");
            ++i;  // pass comma
            while (i < len && std::isspace(cc_fields[i]))
                ++i;
        }
    }


    void parseCacheControl(HttpRequestWrapper& request) {
        std::string cc_fields = request.getField("CACHE-CONTROL");
        if (cc_fields == "")
            return;
        char key[128], value[128];
        const int len = cc_fields.size();
        int i = 0;
        while (i < len) {
            std::string token;
            while (i < len && cc_fields[i] != ',')
                token.push_back(cc_fields[i++]);
            key[0] = value[0] = 0;
            sscanf(token.c_str(), "%[^=]=%s", key, value);
            request.setField(std::string(key), strlen(value) ? std::string(value) : "true");
            ++i;  // pass comma
            while (i < len && std::isspace(cc_fields[i]))
                ++i;
        }
    }

    std::string toUpper(std::string s) const {
        std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::toupper(c); } // correct
                  );
        return s;
    }

    void parseRequestHeader(HttpRequestWrapper &request)
    {
        char key[128], value[1024];
        while (true)
        {
            std::string next_line = readLine();
            request.appendRawData(next_line);
            if (next_line == "\r\n")
                break;
            key[0] = value[0] = 0; // added reset
            sscanf(next_line.c_str(), "%[^:]: %[^\r]", key, value);
            request.setField(toUpper(std::string(key)), std::string(value));
            if (toUpper(std::string(key)) == "HOST")
            {
                char host[128] = {0}, port[8] = {0};
                sscanf(value, "%[^:]:%s", host, port);
                request.setHost(std::string(host));
                if (strlen(port) == 0)
                    request.setPort("80");
                else
                    request.setPort(std::string(port));
            }
        }
    }
    void parseResponseHeader(HttpResponse &response)
    {
        char key[128], value[4096];
        while (true)
        {
            std::string next_line = readLine();
            response.appendRawData(next_line);
            if (next_line == "\r\n")
                break;
            key[0] = value[0] = 0; // added reset
            sscanf(next_line.c_str(), "%[^:]: %[^\r]", key, value);
            response.setField(toUpper(std::string(key)), std::string(value));
        }
    }

    void parsePayload(HttpRequestWrapper &request)
    {
        std::string content_length = request.getField("CONTENT-LENGTH");
        std::string transfer_encoding_str = request.getField("TRANSFER-ENCODING");
        char buf[MAX_READ + 1];

        if (content_length != "")
        {
            size_t total = std::stoi(content_length);
            while (total)
            {
                buf[0] = 0;
                size_t cnt = rio_readnb(&rio, buf, std::min(total, (size_t)MAX_READ));
                request.appendRawData(buf, cnt);
                total -= cnt;
            }
        }
        else if (transfer_encoding_str.find("chunked") != std::string::npos)
        {
            while (1)
            {
                std::string s = readLine();
                request.appendRawData(s);
                if (s == "0\r\n") {
                    request.appendRawData(readLine());
                    return;
                }
                s.pop_back(); s.pop_back();
                size_t cnt = rio_readnb(&rio, buf, std::stoi(s, 0, 16));
                request.appendRawData(buf, cnt);
                request.appendRawData(readLine());
            }
            return;
        }
    }
    void parsePayload(HttpResponse &response)
    {
        // 1.Any response to a HEAD request or 1xx or 204 or 304, no payload
        int status = std::stoi(response.getField("STATUS"));
        if (status == 204 || status == 304 || status / 100 == 1)
        {
            return;
        }
        // 2.if reponse to connect with 200, no content-length or transfer-encoding, to be completed elsewhere
        // 3.if transfer-encoding is present and chunked transfer coding
        //  is the final encoding, the message
        //  body length is determined by reading and decoding the chunked
        //  data until the transfer coding indicates the data is complete.
        std::string transfer_encoding_str = response.getField("TRANSFER-ENCODING");
        std::string content_length_str = response.getField("CONTENT-LENGTH");
        char buf[MAX_READ + 1];
        memset(buf, 0, MAX_READ + 1);

        if (content_length_str != "")
        {
            size_t content_length = std::stoi(content_length_str);
            while (content_length)
            {
                buf[0] = 0;
                size_t cnt = rio_readnb(&rio, buf, std::min(content_length, (size_t)MAX_READ));
                response.appendRawData(buf, cnt);
                content_length -= cnt;
            }
            return;
        }
        else if (transfer_encoding_str.find("chunked") != std::string::npos)
        {
            while (1) {
            std::string s = readLine();
            response.appendRawData(s);
            if (s == "0\r\n") {
                response.appendRawData(readLine());
                return;
            }
            s.pop_back(); s.pop_back();
            size_t cnt = rio_readnb(&rio, buf, std::stoi(s, 0, 16));
            response.appendRawData(buf, cnt);
            response.appendRawData(readLine());
            }
        }
    }

public:
    Socket(int fd) : fd(fd)
    {
        rio_readinitb(&rio, fd);
        unique_id = -1;
    }
    Socket(int fd, int unique_id) : fd(fd), unique_id(unique_id) {}
	Socket(Socket&& that) {
		this->fd = that.fd;
        this->unique_id = that.unique_id;
        rio_readinitb(&rio, fd);
		that.fd = -1;
        that.unique_id = -1;
	}

    Socket(Socket &that) = delete;
    Socket(const Socket &that) = delete;

    HttpRequestWrapper recvRequest()
    {
        char buf1[128], buf2[1024], buf3[128];
        std::vector<char> raw_data;
        std::string next_line = readLine();
        if (next_line == "") 
            return HttpRequestWrapper(-1);
        sscanf(next_line.c_str(), "%s %s %s", buf1, buf2, buf3);
        HttpRequestWrapper request(unique_id);
        request.appendRawData(next_line);
        request.setField("METHOD", toUpper(std::string(buf1)));
        request.setUrl(std::string(buf2));
        parseRequestHeader(request);
        parsePayload(request);
        parseCacheControl(request);
        Logger* logger = Logger::getLogger();
        std::string ipAddress = getIpAddress();
        logger->logRequest(request,ipAddress);
        return request;
    }

    HttpResponse recvResponse()
    {
        char buf1[128], buf2[1024], buf3[128];
        std::vector<char> raw_data;
        std::string next_line = readLine();
        if (next_line == "")
            return HttpResponse();
        sscanf(next_line.c_str(), "%s %s %s", buf1, buf2, buf3);
        HttpResponse response;
        response.appendRawData(next_line);
        response.setField("STATUS", std::string(buf2));
        parseResponseHeader(response);
        parsePayload(response);
        parseCacheControl(response);
        
        return response;
    }

    void sendRequest(const HttpRequestWrapper &request)
    {
        rio_writen(fd, request.getRawData().data(), request.getRawData().size());
    }

    void sendResponse(const HttpResponse &response)
    {
        rio_writen(fd, response.getRawData().data(), response.getRawData().size());
    }

    void sendRawData(const char *raw_data, size_t size)
    {
        rio_writen(fd, raw_data, size);
    }

    int getFileDescriptor()
    {
        return fd;
    }

    ~Socket()
    {
        if (fd >= 0)
            close(fd);
    }
    
    std::string getIpAddress(){
        sockaddr_in addr;
        socklen_t addr_size = sizeof(sockaddr_in);
        getpeername(fd, (struct sockaddr *)&addr, &addr_size);
        char *clientip = new char[20];
        strcpy(clientip, inet_ntoa(addr.sin_addr));
        std::string address(clientip);
        return address;

    }
};

#endif
