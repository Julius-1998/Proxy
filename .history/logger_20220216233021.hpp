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
    Logger(const std::string location):file_location(location){}
    ~Logger(){}
    static std::string file_location;
public:
    Logger(Logger & other) = delete;
    void operator=(const Logger &) = delete;
    static Logger * getLogger(const std::string & location);
    static void log(std::string log);
    std::string location() const{
        return file_location;
    }
};
Logger * Logger::logger{nullptr};
std::mutex Logger::logger_lock;

Logger * Logger::getLogger(const std::string &location){
    std::lock_guard<std::mutex> lock(logger_lock);
    if(logger == nullptr){
        logger = new Logger(location);
    }
    return logger;
}

void Logger::log(const std::string log){
    std::lock_guard<std::mutex> lock(logger_lock);
    std::ofstream myfile;
    myfile.open(file_location);
    myfile<<log<<std::endl;
    myfile.close();
}

#endif