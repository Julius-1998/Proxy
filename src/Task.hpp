#ifndef __TASK_HPP__
#define __TASK_HPP__

#include "HttpCache.hpp"
HttpCache Cache(16, 8);

class Task
{
private:
    Socket in;
    Socket out;
    HttpRequestWrapper request;

public:
    Task(Socket &&in, Socket &&out, HttpRequestWrapper &&request) : in(std::move(in)), out(std::move(out)), request(std::move(request))
    {
    }

    void execute()
    {
        if (request.getField("METHOD") == "CONNECT")
        {
            HttpResponse response;
            response.appendRawData("HTTP1.1 200 OK\r\n\r\n");
            in.sendResponse(response);
            BlindForwarder forwarder(std::move(in), std::move(out));
            forwarder.forward();
            Logger* logger = Logger::getLogger();
            logger->logCache(request, "Tunnel closed");
        }
        else if (request.getField("METHOD") == "GET")
        {
            Logger *logger = Logger::getLogger();
            auto optional_response = Cache.get(request, out);
            if (optional_response.has_value())
            {
                in.sendResponse(optional_response.value());
                return;
            }
            logger->logContactingServerRequest(request);
            out.sendRequest(request);
            HttpResponse response = out.recvResponse();
            logger->logContactingServerResponse(request, response);

            if (response.getField("STATUS") == "200")
            {
                Cache.put(request, response);
            }
            in.sendResponse(response);
        }
        else if (request.getField("METHOD") == "POST")
        {
            Logger *logger = Logger::getLogger();
            logger->logContactingServerRequest(request);
            out.sendRequest(request);
            HttpResponse response = out.recvResponse();
            logger->logContactingServerResponse(request, response);
            in.sendResponse(response);
        }
    }


};

#endif
