cmake_minimum_required(VERSION 3.2)
project(demo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(demo ../include)
link_directories(demo ../Release/libwtfdanmaku.Desktop)

set(SOURCE_FILES main.c)

add_executable(demo ${SOURCE_FILES})

target_link_libraries(demo libwtfdanmaku)