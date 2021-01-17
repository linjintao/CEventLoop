

#include_directories("../ThirdParty/llhttp/include")
#find_library(HttpParser llhttp)
add_executable(HttpServer "main.cc" "HttpServer.cc" "HttpRequest.cc" "HttpResponse.cc" "HttpContext.cc")
target_link_libraries(HttpServer llhttp CEventLoop ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(HttpServer llhttp)