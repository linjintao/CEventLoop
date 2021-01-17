#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#include <string_view>
#include <functional>
#include <vector>
#include <thread>

namespace CEventLoop
{
    enum class LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    class Logger
    {
    public:
        using OutputFunc = std::function<void(const char*, int)>;
        using FlushFunc = std::function<void()>;

        static LogLevel logLevel();
        static void setLogLevel(LogLevel level);
        static void setOutputFunc(OutputFunc func);
        static void setFlushFunc(FlushFunc func);

        Logger(std::string_view sourceFile, int line);
        ~Logger();

        Logger& operator<<(std::string_view log);
        Logger& operator<<(void* p);
        Logger& operator<<(const char* s);
        Logger& operator<<(int i);
        Logger& operator<<(std::thread::id id);
        
    private:
        std::vector<char> buffer_;
        std::string_view sourceFile_;
        int line_;

    };

    extern LogLevel g_logLevel;

    inline LogLevel Logger::logLevel()
    {
        return g_logLevel;
    }


#define LOG_TRACE if(CEventLoop::Logger::logLevel() ==  CEventLoop::LogLevel::TRACE) \
                    CEventLoop::Logger(__FILE__, __LINE__)

#define LOG_DEBUG if(CEventLoop::Logger::logLevel() <=  CEventLoop::LogLevel::DEBUG) \
                    CEventLoop::Logger(__FILE__, __LINE__)

#define LOG_INFO if(CEventLoop::Logger::logLevel() <=  CEventLoop::LogLevel::INFO) \
                    CEventLoop::Logger(__FILE__, __LINE__)

#define LOG_WARN if(CEventLoop::Logger::logLevel() <=  CEventLoop::LogLevel::INFO) \
                    CEventLoop::Logger(__FILE__, __LINE__)

#define LOG_SYSERR if(Logger::logLevel() <=  LogLevel::ERROR) \
                    CEventLoop::Logger(__FILE__, __LINE__)

#define LOG_FATAL if(Logger::logLevel() <=  LogLevel::FATAL) \
                    CEventLoop::Logger(__FILE__, __LINE__)
}


#endif