cmake_minimum_required(VERSION 3.0.0)
project(syncpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB src 
"include/syncpp/*.hpp"
"include/syncpp/private/*.hpp"
)

add_library(syncpp STATIC ${src})

