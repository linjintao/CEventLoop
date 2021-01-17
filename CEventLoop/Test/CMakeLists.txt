

add_executable(TestPoller "TestPoller.cc" "TestPoller2.cc")
target_link_libraries(TestPoller CEventLoop)

add_test(NAME test_poller COMMAND TestPoller)



add_executable(TestEventLoop "TestEventLoop.cc")
target_link_libraries(TestEventLoop CEventLoop ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME test_eventloop COMMAND TestEventLoop)


add_executable(TestTcpClient "TestTcpClient.cc")
target_link_libraries(TestTcpClient CEventLoop)
add_test(NAME test_tcpclient COMMAND TestTcpClient)

add_executable(TestEventLoopThreadPool "TestEventLoopThreadPool.cc")
target_link_libraries(TestEventLoopThreadPool CEventLoop ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME test_eventloopthreadpool COMMAND TestEventLoopThreadPool)

add_subdirectory("echo")