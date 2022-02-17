#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include<thread>
#include<mutex>
class logger
{
private:
    std::mutex m;

public:
    logger(/* args */);
    ~logger();
};

logger::logger(/* args */)
{
    
}

logger::~logger()
{
}

#endif