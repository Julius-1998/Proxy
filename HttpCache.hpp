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
        auto opt_res = cache.get(url);
        if (!op_res.has_value()) {
            // log not in cache
            return opt_res;
        }
        if (isExpired(response)) {
            // log expires
        }
        out.send(HttpRequest());
        HttpResponse response = out.recvResponse();
        if (response.getRawData().empty())
            return {};
        if (response.getField("status") == "304") {
            // log revalidate
        }

    }

    void put(const HttpRequest& request, const HttpResponse& response) {
        setAge(response);
        cache.put(url);

    }



};





#endif
