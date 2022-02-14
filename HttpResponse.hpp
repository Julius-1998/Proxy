#ifndef __HTTPRESPONSE_HPP__
#define __HTTPRESPONSE_HPP__
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <iostream>
extern "C" {
    #include "csapp.h"
}

class Socket;
class HttpResponse {
protected:
    std::vector<char> raw_data;
    std::unordered_map<std::string, std::string> header_fields;
public:
	enum STATUS { OK, REDIRECT, NOT_FOUND, INTERNAL_ERROR };
    STATUS status;
    HttpResponse() : status(OK) { }	

    std::string getField(const std::string& field) {
        if (header_fields.count(field))
            return header_fields[field];
        return "";
    }
    const std::vector<char>& getRawData() const { return raw_data; }
    void appendRawData(const std::string& data) { for (char c : data) raw_data.push_back(c); }
    void appendRawData(const char* data) { while (*data) raw_data.push_back(*data++); }
    void setField(const std::string& k, const std::string& v) { header_fields[k] = v; }
    STATUS getStatus() { return status; }

    HttpResponse(HttpResponse&& that) {
       raw_data = std::move(that.raw_data);
       header_fields = std::move(that.header_fields);
    }

    HttpResponse(HttpResponse& that) = delete;
    HttpResponse(const HttpResponse& that) = delete;


};


#endif
