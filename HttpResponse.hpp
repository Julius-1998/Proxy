#ifndef __HTTPRESPONSE_HPP__
#define __HTTPRESPONSE_HPP__
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <iostream>
#include <ctime>
#define HEURISTIC_TIME 86400

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

    std::string isCachable() {
        if (header_fields.getField("no-store") != "")
            return "response specifies no-store"
        if (header_fields.getField("private") != "")
            return "response specifies private";
        return "";
    }

    bool isMaxAgeZero() {
        header_fields.getField("s-maxage") == "0" || header_fields.getField("max-age") == "0";
    }

    bool isAlwaysRevalidate() {
        return header_fields.getField("no-cache") != "" || isMaxAgeZero();
    }
    
    std::string currentTimeInString() {
	    std::time_t t;
	    std::time(&t);
	    auto p = std::gmtime(&t);
	    return std::asctime(p);
    }

    std::time_t currentTimeInSeconds() {
	    std::time_t t;
	    std::time(&t);
	    return t;
    }

    int elapsedSecondsToNow(std::string s) {
	    struct tm* t;
        std::strptime(s.c_str(), "%a, %d %b %Y %T", t);
	    return currentTimeInSeconds() - std::mktime(t);
    }

    std::string getDate() {
        struct tm* t;
        std::strptime(header_fields.getField("DATE"), "%a, %d %b %Y %T", t);
        return std::asctime(t);
    }

    STATUS getStatus() { return status; }

    HttpResponse(HttpResponse&& that) {
       raw_data = std::move(that.raw_data);
       header_fields = std::move(that.header_fields);
    }
// ---------------------------------
    HttpResponse(const HttpResponse& that) {
        raw_data = that.raw_data;
        header_fields = that.header_fields;
    }

    HttpResponse& operator=(const HttpResponse& that) {
        if (this == &that)
            return *this;
        raw_data = that.raw_data;
        header_fields = that.header_fields;
        return *this;
    }
// -----------------------------------
};


#endif
