#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include<thread>
#include<mutex>
#include<iostream>
#include<fstream>
class logger
{
private:
    std::mutex m;
    std::string output = "log.txt";
public:
    logger(/* args */);
    ~logger();
    log(std::string log);
};

logger::logger(/* args */)
{

}

logger::~logger()
{
}

logger::log(std::string log){
    std::ofstream myfile;
    
}

#endif