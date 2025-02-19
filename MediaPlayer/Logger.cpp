#include "pch.h"
#include "Logger.h"

Logger::Logger(const std::string& fileName)
{
    m_logFile.open(fileName, std::ios::app);
    if (!m_logFile.is_open()) {
        std::cerr << "Error opening log file." << '\n';
    }
}

Logger& Logger::getInstance(const std::string fileName)
{
    static Logger m_logger(fileName);

    return m_logger;
}

Logger::~Logger()
{
    m_logFile.close();
}

void Logger::log(LogLevel level, const std::string& logText)
{
    if (m_logFile.is_open())
    {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm nowTm;
        localtime_s(&nowTm, &now);
        char timeBuffer[26];
        asctime_s(timeBuffer, sizeof(timeBuffer), &nowTm);
        timeBuffer[24] = '\0';

        m_logFile << "[" << timeBuffer << "] [" << levelToString(level) << "] " << logText << std::endl;
    }
}

std::string Logger::levelToString(LogLevel level)
{
    switch (level) {
    case DEBUG:
        return "DEBUG";
    case INFO:
        return "INFO";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    case CRITICAL:
        return "CRITICAL";
    default:
        return "UNKNOWN";
    }
}