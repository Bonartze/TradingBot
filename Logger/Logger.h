#ifndef LOGGER_H
#define LOGGER_Hz

#include <sstream>
#include <mutex>
#include <string>
#include <iomanip>

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

    std::string getCurrentTime() const;
    std::string levelToString(LogLevel level) const;

public:
    explicit Logger(LogLevel level);

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

#endif
