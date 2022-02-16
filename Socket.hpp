#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#define MAX_LINE (1 << 16)
#define MAX_READ (1 << 16)

#include <algorithm>

class Socket {
private:
	int fd = -1;
    rio_t rio;

    std::string readLine() {
        char buf[MAX_LINE];
        int cnt = rio_readlineb(&rio, buf, MAX_LINE);
        if (cnt == 0)
            return "";
        if (cnt < 0)
            return "";
            // TODO throw error
        return std::string(buf);
    }

    void parseCacheControl(const HttpResponse& response) {
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
            response.setField(toUpper(std::string(key)), strlen(value) ? std::string(value) : "true");
            ++i;  // pass comma
            while (i < len && std::isspace(cc_fields[i]))
                ++i;
        }
    }

    void parseCacheControl(const HttpResponse& request) {
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
            request.setField(toUpper(std::string(key)), strlen(value) ? std::string(value) : "true");
            ++i;  // pass comma
            while (i < len && std::isspace(cc_fields[i]))
                ++i;
        }
    }

    std::string toUpper(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::toupper(c); } // correct
                  );
        return s;
    }


    void parseRequestHeader(HttpRequestWrapper& request) {
        char key[128], value[1024];
        while (true) {
            std::string next_line = readLine();
            request.appendRawData(next_line);
            if (next_line == "\r\n")
                break;
            key[0] = value[0] = 0; // added reset
            sscanf(next_line.c_str(), "%[^:]: %[^\r]", key, value);
            request.setField(toUpper(std::string(key)), std::string(value));
            if (toUpper(std::string(key)) ==  "HOST") {
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
    void parseResponseHeader(HttpResponse& response) {
        char key[128], value[4096];
        while (true) {
            std::string next_line = readLine();
            response.appendRawData(next_line);
            if (next_line == "\r\n")
                break;
            key[0] = value[0] = 0; // added reset
            sscanf(next_line.c_str(), "%[^:]: %[^\r]", key, value);
            response.setField(toUpper(std::string(key)), std::string(value));
        }
    }

    void parsePayload(HttpRequestWrapper& request) {
        std::string content_length = request.getField("CONTENT-LENGTH");
        if (content_length == "")
            return;
        char buf[MAX_READ + 1];
        size_t total = std::stoi(content_length);
        while (total) {
            size_t cnt = rio_readnb(&rio, buf, std::min(total, (size_t)MAX_READ));
            buf[cnt] = 0;
            request.appendRawData(buf);
            total -= cnt;
        }
        //TODO error handling
    }
    void parsePayload(HttpResponse& response) {
        std::string content_length = response.getField("CONTENT-LENGTH");
        if (content_length == "")
            return;
        char buf[MAX_READ + 1];
        size_t total = std::stoi(content_length);
        while (total) {
            size_t cnt = rio_readnb(&rio, buf, std::min(total, (size_t)MAX_READ));
            buf[cnt] = 0;
            response.appendRawData(buf);
            total -= cnt;
        }
        // TODO error handling
    }
public:
	Socket(int fd) : fd(fd) {
        rio_readinitb(&rio, fd);
    }

	Socket(Socket&& that) {
		this->fd = that.fd;
        rio_readinitb(&rio, fd);
		that.fd = -1;
	}

	Socket(Socket& that) = delete;
	Socket(const Socket& that) = delete;

	HttpRequestWrapper recvRequest() {
        // TODO
        char buf1[128], buf2[1024], buf3[128];
        std::vector<char> raw_data;
        std::string next_line = readLine();
        if (next_line == "") 
            return HttpRequestWrapper("GET");
        sscanf(next_line.c_str(), "%s %s %s", buf1, buf2, buf3);
        HttpRequestWrapper request(std::string((char*)buf1));
        request.appendRawData(next_line);
        request.setUrl(std::string(buf2));
        parseRequestHeader(request);
        parsePayload(request);
        parseCacheControl(request);
        return request;
	}

	HttpResponse recvResponse() {
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
        parseCacheControl(response);
        return response;
	}

	void sendRequest(const HttpRequestWrapper& request) {
        rio_writen(fd, request.getRawData().data(), request.getRawData().size());
	}

	void sendResponse(const HttpResponse& response) {
        rio_writen(fd, response.getRawData().data(), response.getRawData().size());
	}

    void sendRawData(const char* raw_data, size_t size) {
        rio_writen(fd, raw_data, size);
    }

	int getFileDescriptor() {
		return fd;
	}
	
	~Socket() {
		if (fd >= 0)
			close(fd);
	}

};

#endif
