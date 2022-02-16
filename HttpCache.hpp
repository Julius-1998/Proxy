#ifndef __HTTPCACHE_HPP__
#define __HTTPCACHE_HPP__
#include "RawCache.hpp"
#include <string>

class HttpCache {
private:
    ConcurrentRandomEvictCache<std::string, HttpResponse> cache;

    bool isExpired(const HttpResponse& response) {
        return false;
    }
public:
    HttpCache(int max_capacity, int size) : cache(max_capacity, size) { }


    std::optional<HttpResponse> get(const HttpRequest& request, const Socket& out) {
        auto key = request.getCacheKey();
        auto optional_response = cache.get(key);
        if (!op_res.has_value()) {
            // TODO
            // log not in cache 
            return optional_response;
        }
        if (isExpired(response)) {
            // log expires
        }
        out.send(HttpRequest());
        HttpResponse response = out.recvResponse();
        if (response.getRawData().empty())
            return {};
        if (response.getField("STATUS") == "304") {
            // log revalidate
        }

    }

    void put(const HttpRequestWrapper& request, const HttpResponse& response) {
        auto request_cachable_msg = request.isCachable();
        if (request_cachable_msg != "") {
            //TODO
            //log(error_msg) no-store
            return;
        }
        auto response_cachable_msg = response.isCachable();
        if (response_cachable_msg != "") {
            //TODO log
            // 
            //
            return;
        }
        cache.put(request.getCacheKey(), response);
        if (response.isAlwaysRevalidate()) {
            //TODO
            //log in cache but need revalidate
        } else {
            //TODO
            //log cached expires at XXX
            //reponse.getDate();
        }

    }



};





#endif
