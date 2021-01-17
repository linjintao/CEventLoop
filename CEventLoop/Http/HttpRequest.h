#ifndef HTTP_REQUEST_H_INCLUDED
#define HTTP_REQUEST_H_INCLUDED

#include "../Timestamp.h"
#include <map>
#include <string>

namespace CEventLoop
{
    class HttpRequest
    {
    public:
        friend class HttpContext;

        enum class Method
        {
            Get,
            Post,
            Head,
            Put,
            Delete,
            Invalid
        };
        enum class Version
        {
            Unknown,
            Http10,
            Http11
        };

        void setReceiveTime(Timestamp t)
        {
            receiveTime_ = t;
        }

    private:
        void setMethod(Method method)
        {
            method_ = method;
        }

        //TcpClient client_;
        Version version_;
        Method method_;
        std::string path_;
        std::string query_;
        Timestamp receiveTime_;
        std::map<std::string, std::string> headers;

        static std::pair<std::string, Method> requestMethods[];
    };
}


#endif