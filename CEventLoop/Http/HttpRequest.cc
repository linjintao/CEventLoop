
#include "HttpRequest.h"

using namespace CEventLoop;

std::pair<std::string, HttpRequest::Method> HttpRequest::requestMethods[] = 
{
    {"GET",    HttpRequest::Method::Get},
    {"POST",   HttpRequest::Method::Post},
    {"HEAD",   HttpRequest::Method::Head},
    {"Put",    HttpRequest::Method::Put},
    {"Delete", HttpRequest::Method::Delete},
};