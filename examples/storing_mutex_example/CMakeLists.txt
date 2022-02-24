cmake_minimum_required(VERSION 3.0.0)
project(example VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(example "main.cpp")
target_include_directories(example PRIVATE "../../include")