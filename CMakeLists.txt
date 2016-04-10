cmake_minimum_required(VERSION 3.3)
project(Server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES server.c server.h)
add_executable(Server ${SOURCE_FILES})