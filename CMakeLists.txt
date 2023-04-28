cmake_minimum_required(VERSION 3.25)
project(tables)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(tables src/main.cpp)
