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
    static const std::string output;
public:
    static void log(std::string log);
};
const std::string logger::output = "log.txt";


void logger::log(const std::string log){
    std::lock_guard<std::mutex> guard(m);
    std::ofstream myfile;
    myfile.open(output);
    myfile<<log<<std::endl;
    myfile.close();
}

#endif