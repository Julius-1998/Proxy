#ifndef __HTTPRESPONSE_HPP__
#define __HTTPRESPONSE_HPP__
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <iostream>
#include <ctime>
#define HEURISTIC_TIME 86400

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

    std::string getField(const std::string& field) const {
        if (header_fields.count(field))
            return header_fields.at(field);
        return "";
    }
    const std::vector<char>& getRawData() const { return raw_data; }
    void appendRawData(const std::string& data) { for (char c : data) raw_data.push_back(c); }
    void appendRawData(const char* data) { while (*data) raw_data.push_back(*data++); }
    void appendRawData(const char* data, size_t size) { while(size--) raw_data.push_back(*data++);}
    void setField(const std::string& k, const std::string& v) { header_fields[k] = v; }

    std::string isCachable() const {
        if (getField("no-store") != "")
            return "response specifies no-store";
        if (getField("private") != "")
            return "response specifies private";
        return "";
    }

    bool isMaxAgeZero() const {
        return getField("s-maxage") == "0" || getField("max-age") == "0";
    }

    bool isAlwaysRevalidate() const {
        return getField("no-cache") != "" || isMaxAgeZero();
    }
    
    std::string currentTimeInString() const {
	    std::time_t t;
	    std::time(&t);
	    auto p = std::gmtime(&t);
	    return std::asctime(p);
    }

    std::time_t currentTimeInSeconds() const {
	    std::time_t t;
	    std::time(&t);
	    return t;
    }

    int elapsedSecondsToNow(const std::string& s) const {
	    struct tm t;
        strptime(s.c_str(), "%a, %d %b %Y %T", &t);
	    return currentTimeInSeconds() - std::mktime(&t);
    }
    bool isExpired() const {
        if (getField("max-age") != "" || getField("s-maxage") != "")
            return false;
        std::string response_time = getField("DATE");
        std::string expire_time = getField("EXPIRES");
        if (expire_time != "") 
            return elapsedSecondsToNow(expire_time) >= 0;
        return elapsedSecondsToNow(response_time) >= HEURISTIC_TIME;
    }

    bool isRevalidatable() const {
        return getField("LAST-MODIFIED") != "" || getField("ETAG") != "";
    }
    bool needsRevalidation() const {
        if (isAlwaysRevalidate())
            return true;
        if (getField("CACHE-CONTROL") == "")
            return false;
        if (getField("s-maxage") != "")
            return elapsedSecondsToNow(getField("DATE")) >= std::stoi(getField("s-maxage")) + std::stoi(getField("max-stale") + "0") / 10 - std::stoi(getField("min-fresh") + "0") / 10;
        if (getField("max-age") != "")
            return elapsedSecondsToNow(getField("DATE")) >= std::stoi(getField("max-age")) + std::stoi(getField("max-stale") + "0") / 10 - std::stoi(getField("min-fresh") + "0") / 10;
        if (getField("must-revalidate") != "")
            return true;
        if (getField("proxy-revalidate") != "")
            return true;
        return false;
    }
    std::string getDateString() const {
        struct tm t;
        strptime(getField("DATE").c_str(), "%a, %d %b %Y %T", &t);
        return std::asctime(&t);
    }

    struct tm dateStringToTime(const std::string& s) const {
        struct tm t;
        strptime(s.c_str(), "%a, %d %b %Y %T", &t);
        return t;
    }

    std::time_t dateStringToSeconds(std::string s) const {
        struct tm t = dateStringToTime(s);
        return std::mktime(&t);
    }

    std::string secondsToDateString(std::time_t t) const {
        auto p = std::gmtime(&t);
        return std::asctime(p);
    }

    std::string getExpiringDateString() const {
        std::time_t s = dateStringToSeconds(getField("DATE"));
        if (getField("s-maxage") != "") {
            s = s + std::stoi(getField("s-maxage")) + std::stoi(getField("max-stale") + "0") / 10 - std::stoi(getField("min-fresh") + "0") / 10;
        } else if (getField("max-age") != "") {
            s = s + std::stoi(getField("s-maxage")) + std::stoi(getField("max-stale") + "0") / 10 - std::stoi(getField("min-fresh") + "0") / 10;
        } else if (getField("EXPIRES") != "") {
            s = dateStringToSeconds(getField("EXPIRES"));
        } else {
            s = s + HEURISTIC_TIME;
        }
        return secondsToDateString(s);
    }

    STATUS getStatus() { return status; }
    std::string getResponseLine() const{
        return getStatus;
    }

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
