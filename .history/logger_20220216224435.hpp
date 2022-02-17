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
    std::string output;
public:
    void log(std::string log);
};



void logger::log(const std::string log){
    std::lock_guard<std::mutex> lock(m);
    std::ofstream myfile;
    myfile.open(output);
    myfile<<log<<std::endl;
    myfile.close();
}

#endif