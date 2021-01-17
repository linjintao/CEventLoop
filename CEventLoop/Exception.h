#ifndef EXCEPTION_H_INCLUDED
#define EXCEPTION_H_INCLUDED

#include <exception>
#include <string>

namespace CEventLoop
{
    class Exception : public std::exception
    {
    public:
        Exception(std::string what);

        const char* stackTrace() const noexcept
        {
            return stack_.c_str();
        }

        const char* what() const noexcept
        {
            return message_.c_str();
        }

    private:
        std::string GetStackTrace() const noexcept;

        std::string message_;
        std::string stack_;
    };
}



#endif