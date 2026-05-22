cmake_minimum_required(VERSION 4.2)
project(thread)

set(CMAKE_CXX_STANDARD 20)

add_executable(thread
        threadpool.h
        threadpool.cpp
        main.cpp
)
