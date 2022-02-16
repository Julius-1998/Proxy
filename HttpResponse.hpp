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
    bool isExpired() {
        if (header_fields.getField("max-age") != "" || header_fields.getField("s-maxage") != "")
            return false;
        std::string response_time = header_fields.getField("DATE");
        std::string expire_time = header_fields.getField("EXPIRES");
        if (expire_time != "") 
            return elapsedSecondsToNow(expire_time) >= 0;
        return elapsedSecondsToNow(response_time) >= HEURISTIC_TIME;
    }

    bool isRevalidatable() {
        return header_fields.getField("LAST-MODIFIED") != "" || header_fields.getField("ETAG") != "";
    }
    bool needsRevalidation() {
        if (isAlwaysRevalidate())
            return true;
        if (header_fields.getField("CACHE-CONTROL") == "")
            return false;
        if (header_fields.getField("s-maxage") != "")
            return elapsedSecondsToNow(getDate()) >= std::stoi(header_fields.getField("s-maxage")) + std::stoi(header_fields.getField("max-stale") + "0") / 10 - std::stoi(header_fields.getField("min-fresh") + "0") / 10;
        if (header_fields.getField("max-age") != "")
            return elapsedSecondsToNow(getDate()) >= std::stoi(header_fields.getField("maxage")) + std::stoi(header_fields.getField("max-stale") + "0") / 10 - std::stoi(header_fields.getField("min-fresh") + "0") / 10;
        if (header_fields.getField("must-revalidate") != "")
            return true;
        if (header_fields.getField("proxy-revalidate") != "")
            return true;
        return false;
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
