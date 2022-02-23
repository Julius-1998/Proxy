#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>
class Logger
{
private:
    static std::mutex logger_lock;
    static Logger *logger;
    std::ofstream ofstream;

protected:
    Logger()
    {
        file_location = "/var/log/erss/proxy.log";
        ofstream.open(file_location);
    }
    Logger(const std::string location) : file_location(location) {}
    ~Logger() {ofstream.close();}
    std::string file_location;

public:
    Logger(Logger &other) = delete;
    void operator=(const Logger &) = delete;
    static Logger *getLogger();
    void log(std::string log);
    std::string location() const
    {
        return file_location;
    }
    void logRequest(HttpRequestWrapper &request, std::string IpAddress);
    void logCache(const HttpRequestWrapper &request, std::string state);
    void logContactingServerRequest(const HttpRequestWrapper &request);
    void logContactingServerResponse(const HttpRequestWrapper &request, const HttpResponse &response);
};
Logger *Logger::logger{nullptr};
std::mutex Logger::logger_lock;

Logger *Logger::getLogger()
{
    std::lock_guard<std::mutex> lock(logger_lock);
    if (logger == nullptr)
    {
        logger = new Logger();
    }
    return logger;
}

void Logger::log(const std::string log)
{
    std::lock_guard<std::mutex> lock(logger_lock);
    ofstream << log << std::endl;
}

void Logger::logRequest(HttpRequestWrapper &request, std::string IpAddress)
{
    time_t curr_time = time(NULL);
    char *tm = ctime(&curr_time);
    std::string time(tm);
    time.pop_back();
    std::string idString = std::to_string(request.getUniqueId());
    std::string logString = idString + ": " + request.get()->getRequestLine() + " from " + IpAddress + " @ " + time;
    log(logString);
}

void Logger::logCache(const HttpRequestWrapper &request, std::string state)
{
    // state should be the case;
    std::string idString = std::to_string(request.getUniqueId());
    std::string logString = idString + ": " + state;
    log(logString);
}

void Logger::logContactingServerRequest(const HttpRequestWrapper &request)
{
    std::string idString = std::to_string(request.getUniqueId());
    std::string logString = idString + ": Requesting " + request.getRequestLine() + " from " + request.getHost();
    log(logString);
}

void Logger::logContactingServerResponse(const HttpRequestWrapper &request, const HttpResponse &response)
{
    std::string idString = std::to_string(request.getUniqueId());
    std::string logString = idString + ": Received " + response.getResponseLine() + " from " + request.getHost();
    log(logString);
}

#endif
