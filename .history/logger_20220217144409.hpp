#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include<thread>
#include<mutex>
#include<iostream>
#include<fstream>
class Logger
{
private:
    static std::mutex logger_lock;
    static Logger * logger;
protected:
    Logger(){file_location = "log.txt";}
    Logger(const std::string location):file_location(location){}
    ~Logger(){}
    std::string file_location;
public:
    Logger(Logger & other) = delete;
    void operator=(const Logger &) = delete;
    static Logger * getLogger();
    void log(std::string log);
    std::string location() const{
        return file_location;
    }
    void logRequest(HttpRequestWrapper& request,std::string IpAddress);
};
Logger * Logger::logger{nullptr};
std::mutex Logger::logger_lock;

Logger * Logger::getLogger(){
    std::lock_guard<std::mutex> lock(logger_lock);
    if(logger == nullptr){
        logger = new Logger();
    }
    return logger;
}

void Logger::log(const std::string log){
    std::lock_guard<std::mutex> lock(logger_lock);
    std::ofstream myfile;
    myfile.open(file_location,std::ios::app);
    myfile<<log<<std::endl;
    myfile.close();
}

void Logger::logRequest( HttpRequestWrapper &request, std::string IpAddress){
    time_t curr_time = time(NULL);
	char *tm = ctime(&curr_time);
    std::string time(tm);
    std::string idString = std::to_string(request.getUniqueId());
    std::string logString = idString + ": "+ request.get()->getRequestLine() 
    + " from " + IpAddress + " @ "+time;
    log(logString);
}

void Logger::logCache( HttpRequestWrapper &request, std::string state){
    //state should be the case;
    std::string idString = std::to_string(request.getUniqueId());
    std::string logString = idString + ": " + state; 
    log(logString);
}
#endif