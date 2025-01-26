#pragma once
#include <sstream>
#include <mutex>
#include <string>
#include <iomanip>
#include <fstream>
#include <iostream>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger {
    LogLevel m_level;
    std::ostringstream m_buffer;
    static LogLevel s_currentLevel;
    static std::mutex s_mutex;
    std::ostream& m_stream;

    std::string getCurrentTime() const;
    std::string levelToString(LogLevel level) const;

public:
    explicit Logger(LogLevel level, std::ostream& stream_ = std::cerr);

    template <typename T>
    Logger& operator<<(const T& value) {
        if (m_level >= s_currentLevel) {
            m_buffer << value;
        }
        return *this;
    }

    ~Logger();

    static void setLogLevel(LogLevel level);
};

