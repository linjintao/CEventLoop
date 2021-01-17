

add_executable(echo "echo.cc" "main.cc")
target_link_libraries(echo CEventLoop ${CMAKE_THREAD_LIBS_INIT})
