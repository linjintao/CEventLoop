#include "HttpContext.h"
#include "../Buffer.h"
#include <string.h>

int onMessageConplete(llhttp_t* llhttp)
{
    auto httpContext = reinterpret_cast<CEventLoop::HttpContext*>(llhttp->data);
    httpContext->getRequestCallback()(httpContext->getRequest());
    return 0;
}

CEventLoop::HttpContext::HttpContext()
{
    llhttp_settings_init(&settings_);
    settings_.on_message_complete = onMessageConplete;
    llhttp_init(&parser_, HTTP_BOTH, &settings_);
    parser_.data = this;
}

bool CEventLoop::HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime)
{
    bool ok = true;
    bool hasMore = true;
    while (hasMore)
    {
        if (state_ == HttpRequestParseState::ExpectRequestLine)
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                ok = processRequestLine(buf->peek(), crlf);
                if (ok)
                {
                    request_.setReceiveTime(receiveTime);
                    buf->retrieveUtil(crlf + 2);
                    state_ = HttpRequestParseState::ExpectHeaders;
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if (state_ == HttpRequestParseState::ExpectHeaders)
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                const char* colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf)
                {
                    request_.headers[std::string(buf->peek(), colon)] = std::string(colon + 1, crlf);
                }
                else
                {
                    state_ = HttpRequestParseState::GotAll;
                    hasMore = false;
                }
                buf->retrieveUtil(crlf + 2);
            }

        }

    }
    return ok;
}

void CEventLoop::HttpContext::execute(std::string_view request)
{
    llhttp_execute(&parser_, request.data(), request.length());
}

bool CEventLoop::HttpContext::processRequestLine(const char* begin, const char* end)
{
    bool succeed = false;
    std::string line(begin, end);
    char method[12], path[1024];
    float version;
    int ret = sscanf(line.c_str(), "%s %s HTTP/%f", method, path, &version);
    request_.setMethod(HttpRequest::Method::Invalid);
    for (int i = 0; i < static_cast<int>(HttpRequest::Method::Invalid); i++)
    {
        if (strcmp(HttpRequest::requestMethods[i].first.c_str(), method) == 0)
        {
            request_.setMethod(HttpRequest::requestMethods[i].second);
            break;
        }
    }

    char* question = strchr(path, '?');
    if (question != nullptr)
    {
        request_.query_ = std::string(question + 1, path + strlen(path));
        request_.path_ = std::string(path, question);
    }
    else
    {
        request_.path_ = std::string(path);
    }

    if (std::abs(version - 1.0) < 0.001)
        request_.version_ = HttpRequest::Version::Http10;
    else if (std::abs(version - 1.1) < 0.001)
        request_.version_ = HttpRequest::Version::Http11;
    else
        request_.version_ = HttpRequest::Version::Http10;
    return ret == 3;
}
