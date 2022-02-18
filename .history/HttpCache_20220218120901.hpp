#ifndef __HTTPCACHE_HPP__
#define __HTTPCACHE_HPP__
#include "RawCache.hpp"
#include <string>
#include <optional>

class HttpCache
{
private:
    ConcurrentRandomEvictCache<std::string, HttpResponse> cache;

    HttpRequestWrapper createRevalidationRequest(const HttpRequestWrapper &request)
    {
        HttpRequestWrapper revalidation_request(request.getUniqueId());
        revalidation_request.appendRawData(request.getField("METHOD") + " " + request.getUrl() + " " + "HTTP1.1\r\n");
        revalidation_request.appendRawData("Host: " + request.getField("HOST") + "\r\n");
        if (request.getField("LAST-MODIFIED") != "")
            revalidation_request.appendRawData("If-Modified-Since: " + request.getField("LAST-MODIFIED") + "\r\n");
        if (request.getField("ETAG") != "")
            revalidation_request.appendRawData("If-None-Match: " + request.getField("ETAG") + "\r\n");
        revalidation_request.appendRawData("\r\n");
        return revalidation_request;
    }

public:
    HttpCache(int max_capacity, int size) : cache(max_capacity, size) {}

    void put(const HttpRequestWrapper &request, const HttpResponse &response)
    {
        auto request_cachable_msg = request.isCachable();
        if (request_cachable_msg != "")
        {
            // TODO
            // log(error_msg) no-store
            return;
        }
        auto response_cachable_msg = response.isCachable();
        if (response_cachable_msg != "")
        {
            // TODO log
            //
            //
            return;
        }
        cache.put(request.getCacheKey(), response);
        if (response.isAlwaysRevalidate())
        {
            // TODO
            // log in cache but need revalidate
        }
        else
        {
            // TODO
            // log cached expires at XXX
            // reponse.getExpiringDateString();
        }
    }

    std::optional<HttpResponse> get(const HttpRequestWrapper &request, Socket &out)
    {
        auto key = request.getCacheKey();
        auto optional_response = cache.get(key);
        Logger *logger = Logger::getLogger();
        if (!optional_response.has_value())
        {
            logger->logCache(request, "not in cache");
            return optional_response;
        }
        auto response = optional_response.value();
        if (response.needsRevalidation() && response.isRevalidatable())
        {
            logger->logCache(request, "in cache, requires validation");
            logger->logContactingServerRequest(request);
            HttpRequestWrapper revalidation_request = createRevalidationRequest(request);
            out.sendRequest(revalidation_request);
            HttpResponse response = out.recvResponse();
            logger->logContactingServerResponse(request, response);
            if (response.getField("STATUS") == "304")
            {
                // TODO
                logger->logCache(request, "NOTE revalidation sucess");
                return optional_response;
            }
            else if (response.getField("STATUS") == "200")
            {
                // TODO
                logger->logCache(request, "NOTE revalidation sucess");
                put(request, response);
                return {response};
            }
            else
            {
                // TODO
                // ERROR
            }
        }
        if ((response.needsRevalidation() && !response.isRevalidatable()) || response.isExpired())
        {
            std::string expire_time = response.getExpireTime()
            logger->logCache(request, "in cache, but expired at" + expire_time);
            out.sendRequest(request);
            HttpResponse new_response = out.recvResponse();
            if (new_response.getField("STATUS") != "200")
            {
                // TODO throw ERROR
            }
            put(request, new_response);
            return {new_response};
        }

        // TODO
        // log in cache valid
        logger->logCache(request, "in cache, valid");
        return optional_response;
    }
}
}
;

#endif
