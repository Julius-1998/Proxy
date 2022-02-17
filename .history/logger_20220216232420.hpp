#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include<thread>
#include<mutex>
#include<iostream>
#include<fstream>
class logger
{
private:
    static std::mutex m;
    static logger * logger;
protected:
    logger(const std::string location):file_location(location){}
    ~logger(){}
    std::string file_location;
public:
    logger(logger & other) = delete;
    void operator=(const logger &) = delete;
    static logger * getLogger(const std::string & location);
    static void log(std::string log);
    std::string location() const{
        return file_location;
    }
};
logger * logger::loggerInstance{nullptr};
std::mutex logger::logger_lock;

logger * logger::getLogger(const std::string &location){
    std::lock_guard<std::mutex> lock(logger_lock);
    if(loggerInstance == nullptr){
        loggerInstance = new logger(location);
    }
    return loggerInstance;
}

void logger::log(const std::string log){
    m.lock();
    std::ofstream myfile;
    myfile.open(output);
    myfile<<log<<std::endl;
    myfile.close();
    m.unlock();
}

#endif