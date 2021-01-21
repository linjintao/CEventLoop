# CMakeList.txt : CMake project for EventLoop, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories("./ThirdParty/llhttp/include")

add_library(CEventLoop STATIC "Poller.cc"
                              "Exception.cc" 
                              "EventLoop.cc"
                              "Channel.cc"
                              "Logging.cc"
                              "Connector.cc"
                              "InetAddress.cc"
                              "SocketsOps.cc"
                              "TcpServer.cc"
                              "Acceptor.cc"
                              "Socket.cc"
                              "TcpConnection.cc"
                              "Buffer.cc"
                              "TcpClient.cc"
                              "EventLoopThread.cc"
                              "EventLoopThreadPool.cc"
                              "Timestamp.cc"
                              "Timer.cc"
                              "TimerQueue.cc") 
add_subdirectory ("Test")
add_subdirectory("ThirdParty/llhttp")
add_subdirectory("Http")

SET(CMAKE_BUILD_TYPE  Debug)
# TODO: Add tests and install targets if needed.

