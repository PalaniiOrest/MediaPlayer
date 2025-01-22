#pragma once
#include <fstream> 
#include <iostream> 
#include <sstream>

#undef ERROR

enum LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

class Logger
{
public:
    static Logger& getInstance(const std::string fileName);
    ~Logger();
    void log(LogLevel level, const std::string& logText);

private:

    std::ofstream m_logFile;

    Logger(const std::string& fileName);
    std::string levelToString(LogLevel level);
};