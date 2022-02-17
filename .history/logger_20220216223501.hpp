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
    static std::string output;
public:
    logger(/* args */);
    ~logger();
    static void log(std::string log);
};
const logger::output = "log.txt";


logger::logger(/* args */)
{

}

logger::~logger()
{
}

void logger::log(std::string log){
    std::lock_guard<std::mutex> guard(m);
    std::ofstream myfile;
    myfile.open(output);
    myfile<<log<<std::endl;
    myfile.close();
}

#endif