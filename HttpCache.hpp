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
        Logger *logger = Logger::getLogger();
        std::string request_cachable_msg = request.isCachable();
        if (request_cachable_msg != "")
        {
            logger->logCache(request,"not cacacheable because "+ request_cachable_msg);
            return;
        }
        std::string response_cachable_msg = response.isCachable();
        if (response_cachable_msg != "")
        {
            logger->logCache(request,"not cacacheable because "+ response_cachable_msg);
            return;
        }
        cache.put(request.getCacheKey(), response);
        if (response.isAlwaysRevalidate())
        {
            logger->logCache(request,"cached,but requires re-validation");
        }
        else
        {
            logger->logCache(request, "cached, expires at " + response.getExpiringDateString());
        }
    };

    std::optional<HttpResponse> get(const HttpRequestWrapper &request, Socket &out)
    {
        auto key = request.getCacheKey();
        std::optional<HttpResponse> optional_response = cache.get(key);
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
            //Contacting the server to get response
            logger->logContactingServerResponse(request, response);
            if (response.getField("STATUS") == "304")
            {
                logger->logCache(request, "NOTE revalidation sucess");
                return optional_response;
            }
            else if (response.getField("STATUS") == "200")
            {
                logger->logCache(request, "NOTE revalidation sucess");
                put(request, response);
                return {response};
            }
            else
            {
                return {};
            }
        }
        if ((response.needsRevalidation() && !response.isRevalidatable()) || response.isExpired())
        {
            std::string expire_time = response.getExpiringDateString();
            logger->logCache(request, "in cache, but expired at" + expire_time);
            out.sendRequest(request);
            HttpResponse new_response = out.recvResponse();
            if (new_response.getField("STATUS") != "200")
            {
                return {};
            }
            put(request, new_response);
            return {new_response};
        }

        logger->logCache(request, "in cache, valid");
        return optional_response;
    };
};

#endif
