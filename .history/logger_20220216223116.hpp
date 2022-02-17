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
public:
    logger(/* args */);
    ~logger();
    static void log(std::string log);
};

logger::logger(/* args */)
{

}

logger::~logger()
{
}

void logger::log(std::string log){
    std::lock_guard<std::mutex> guard(m);
    std::ofstream myfile;
    myfile.open("log.txt");
    myfile<<log<<std::endl;
    myfile.close();
}

#endif