#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#define MAX_LINE (1 << 16)
#define MAX_READ (1 << 16)

#include <algorithm>
#include <sstream>

class Socket
{
private:
    int fd = -1;
    rio_t rio;

    std::string readLine()
    {
        char buf[MAX_LINE];
        int cnt = rio_readlineb(&rio, buf, MAX_LINE);
        if (cnt == 0)
            return "";
        if (cnt < 0)
            return "";
        // TODO throw error
        return std::string(buf);
    }

    std::string toUpper(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c)
                       { return std::toupper(c); } // correct
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
            sscanf(next_line.c_str(), "%[^:]: %[^\r]", key, value);
            response.setField(toUpper(std::string(key)), std::string(value));
        }
    }

    void parsePayload(HttpRequestWrapper &request)
    {
        std::string content_length = request.getField("CONTENT-LENGTH");
        if (content_length == "")
            return;
        char buf[MAX_READ + 1];
        size_t total = std::stoi(content_length);
        while (total)
        {
            size_t cnt = rio_readnb(&rio, buf, std::min(total, (size_t)MAX_READ));
            buf[cnt] = 0;
            request.appendRawData(buf);
            total -= cnt;
        }
        // TODO error handling
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
        if (content_length_str != "")
        {
            size_t content_length = std::stoi(content_length_str);
            while (content_length)
            {
                size_t cnt = rio_readnb(&rio, buf, std::min(content_length, (size_t)MAX_READ));
                buf[cnt] = 0;
                response.appendRawData(buf);
                content_length -= cnt;
            }
            return;
        }
        else if (transfer_encoding_str != "")
        {
            std::stringstream ss(transfer_encoding_str);
            std::string s;
            std::vector<std::string> encodings;
            while (std::getline(ss, s, ','))
            {
                encodings.push_back(s);
            }
            while (1)
            {
                size_t cnt = rio_readlineb(&rio, buf, (size_t)MAX_READ);
                printf("recv()'d %ld from chunked data\n", cnt);
                if(cnt == 3){
                    printf("Break");
                }
                if(cnt <= 0) return;
                response.appendRawData(buf);
            }
            return;
        }
        // TODO error handling
    }

public:
    Socket(int fd) : fd(fd)
    {
        rio_readinitb(&rio, fd);
    }

    Socket(Socket &&that)
    {
        this->fd = that.fd;
        rio_readinitb(&rio, fd);
        that.fd = -1;
    }

    Socket(Socket &that) = delete;
    Socket(const Socket &that) = delete;

    HttpRequestWrapper recvRequest()
    {
        // TODO
        char buf1[128], buf2[1024], buf3[128];
        std::vector<char> raw_data;
        std::string next_line = readLine();
        if (next_line == "")
            return HttpRequestWrapper("GET");
        sscanf(next_line.c_str(), "%s %s %s", buf1, buf2, buf3);
        HttpRequestWrapper request(std::string((char *)buf1));
        request.appendRawData(next_line);
        request.setUrl(std::string(buf2));
        parseRequestHeader(request);
        parsePayload(request);
        return request;
    }

    HttpResponse recvResponse()
    {
        // TODO
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
};

#endif
