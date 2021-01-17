

#include "Logging.h"
#include <stdlib.h>
#include <algorithm>
#include <strstream>

using namespace CEventLoop;

LogLevel initLogLevel()
{
    if (::getenv("CEVENTLOOP_LOG_TRACE"))
        return LogLevel::TRACE;
    else if (::getenv("CEVENTLOOP_LOG_DEBUG"))
        return LogLevel::DEBUG;
    else
        return LogLevel::INFO;
}

void defaultOutput(const char* msg, int len)
{
    size_t n = fwrite(msg, 1, len, stdout);
    //FIXME check n
    (void)n;
}

void defaultFlush()
{
    fflush(stdout);
}



LogLevel CEventLoop::g_logLevel = initLogLevel();
Logger::OutputFunc g_outputFunc = defaultOutput;
Logger::FlushFunc  g_flushFunc = defaultFlush;

void Logger::setLogLevel(LogLevel level)
{
    g_logLevel = level;
}

void Logger::setOutputFunc(OutputFunc func)
{
    g_outputFunc = func;
}

void Logger::setFlushFunc(FlushFunc func)
{
    g_flushFunc = func;
}


Logger::Logger(std::string_view sourceFile, int line) : sourceFile_(sourceFile), line_(line)
{

}

Logger::~Logger()
{
    g_outputFunc(sourceFile_.data(), sourceFile_.length());
    g_outputFunc(":", 1);
    g_outputFunc(buffer_.data(), buffer_.size());
    g_outputFunc("\n", 1);
    g_flushFunc();
}

Logger& Logger::operator<<(std::string_view log)
{
    std::copy(log.begin(), log.end(), std::back_inserter(buffer_));
    return *this;

}

Logger& Logger::operator<<(const char* s)
{
    (*this) << std::string(s);
    return *this;
}

Logger& Logger::operator<<(void* p)
{
    char buf[512];
    snprintf(buf, 512, "%p", p);
    (*this) << buf;
    return *this;
}

Logger& Logger::operator<<(int i)
{
    char buf[512];
    snprintf(buf, 512, "%d", i);
    (*this) << buf;
    return *this;
}

Logger& Logger::operator<<(std::thread::id id)
{
    std::strstream stream;
    stream << id;
    (*this) << stream.str();
    return *this;
}