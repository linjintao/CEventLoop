#ifndef HTTP_CONTEXT_H_INCLUDED
#define HTTP_CONTEXT_H_INCLUDED

#include <functional>
#include <string_view>
#include "llhttp.h"
#include "HttpRequest.h"

namespace CEventLoop
{
    class HttpContext;
    class Buffer;
    using RequestCallback = std::function<void(HttpRequest*)>;
    class HttpContext
    {
    public:
        enum class HttpRequestParseState
        {
            ExpectRequestLine,
            ExpectHeaders,
            ExpectBody,
            GotAll
        };

        HttpContext();

        bool parseRequest(Buffer* buf, Timestamp receiveTime);
        void execute(std::string_view request);
        RequestCallback getRequestCallback() const { return cb_; }
        void setRequestCallback(RequestCallback cb)
        {
            cb_ = cb;
        }

        HttpRequest* getRequest()
        {
            return &request_;
        }

        bool gotAll() const
        {
            return state_ == HttpRequestParseState::GotAll;
        }

    private:
        bool processRequestLine(const char* begin, const char* end);

        RequestCallback cb_;
        llhttp_t parser_;
        llhttp_settings_t settings_;
        HttpRequest request_;
        HttpRequestParseState state_ = HttpRequestParseState::ExpectRequestLine;
    };
}

#endif