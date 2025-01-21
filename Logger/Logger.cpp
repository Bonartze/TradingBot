#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>

LogLevel Logger::s_currentLevel = LogLevel::INFO;
std::mutex Logger::s_mutex;

Logger::Logger(LogLevel level, std::ostream& stream_) : m_level(level), m_stream(stream_) {
    if (m_level >= s_currentLevel) {
        m_buffer << levelToString(level) << " [" << getCurrentTime() << "] ";
    }
}

Logger::~Logger() {
    if (m_level >= s_currentLevel) {
        std::lock_guard<std::mutex> lock(s_mutex);
        m_stream << m_buffer.str() << std::endl;
    }
}

void Logger::setLogLevel(LogLevel level) {
    s_currentLevel = level;
}

std::string Logger::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_time);

    std::ostringstream oss;
    oss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::INFO: return "[INFO]";
        case LogLevel::WARNING: return "[WARNING]";
        case LogLevel::ERROR: return "[ERROR]";
        case LogLevel::DEBUG: return "[DEBUG]";
        default: return "[UNKNOWN]";
    }
}
