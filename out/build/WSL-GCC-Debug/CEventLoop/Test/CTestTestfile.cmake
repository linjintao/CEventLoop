# CMake generated Testfile for 
# Source directory: /mnt/d/linux_projects/CEventLoop/CEventLoop/Test
# Build directory: /mnt/d/linux_projects/CEventLoop/out/build/WSL-GCC-Debug/CEventLoop/Test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_poller "/mnt/d/linux_projects/CEventLoop/out/build/WSL-GCC-Debug/CEventLoop/Test/TestPoller")
set_tests_properties(test_poller PROPERTIES  _BACKTRACE_TRIPLES "/mnt/d/linux_projects/CEventLoop/CEventLoop/Test/CMakeLists.txt;6;add_test;/mnt/d/linux_projects/CEventLoop/CEventLoop/Test/CMakeLists.txt;0;")
add_test(test_eventloop "/mnt/d/linux_projects/CEventLoop/out/build/WSL-GCC-Debug/CEventLoop/Test/TestEventLoop")
set_tests_properties(test_eventloop PROPERTIES  _BACKTRACE_TRIPLES "/mnt/d/linux_projects/CEventLoop/CEventLoop/Test/CMakeLists.txt;12;add_test;/mnt/d/linux_projects/CEventLoop/CEventLoop/Test/CMakeLists.txt;0;")
add_test(test_tcpclient "/mnt/d/linux_projects/CEventLoop/out/build/WSL-GCC-Debug/CEventLoop/Test/TestTcpClient")
set_tests_properties(test_tcpclient PROPERTIES  _BACKTRACE_TRIPLES "/mnt/d/linux_projects/CEventLoop/CEventLoop/Test/CMakeLists.txt;17;add_test;/mnt/d/linux_projects/CEventLoop/CEventLoop/Test/CMakeLists.txt;0;")
add_test(test_eventloopthreadpool "/mnt/d/linux_projects/CEventLoop/out/build/WSL-GCC-Debug/CEventLoop/Test/TestEventLoopThreadPool")
set_tests_properties(test_eventloopthreadpool PROPERTIES  _BACKTRACE_TRIPLES "/mnt/d/linux_projects/CEventLoop/CEventLoop/Test/CMakeLists.txt;21;add_test;/mnt/d/linux_projects/CEventLoop/CEventLoop/Test/CMakeLists.txt;0;")
subdirs("echo")
