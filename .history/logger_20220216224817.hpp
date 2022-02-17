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
    static void log(std::string log);
};
std::string logger::output = "log.txt";
std::mutex logger::m = std::mutex;

void logger::log(const std::string log){
    std::lock_guard<std::mutex> lock(m);
    std::ofstream myfile;
    myfile.open(output);
    myfile<<log<<std::endl;
    myfile.close();
}

#endif