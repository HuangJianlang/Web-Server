cmake_minimum_required(VERSION 3.15)
project(Web_Server)

set(CMAKE_CXX_STANDARD 11)

add_executable(Web_Server main.cpp my_handy_web/udp.cpp my_handy_web/udp.h my_handy_web/util.cpp my_handy_web/util.h my_handy_web/poller.cpp my_handy_web/poller.h my_handy_web/handy_impl.h my_handy_web/handy.h my_handy_web/event_base.cpp my_handy_web/event_base.h my_handy_web/threads.cpp my_handy_web/threads.h my_handy_web/slice.cpp my_handy_web/slice.h my_handy_web/port_posix.cpp my_handy_web/port_posix.h my_handy_web/net.cpp my_handy_web/net.h)