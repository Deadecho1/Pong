cmake_minimum_required(VERSION 3.20)
project(pong)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)

add_executable(pong
        src/main.cpp
        src/input.h)

target_link_libraries(pong PRIVATE SDL3::SDL3)