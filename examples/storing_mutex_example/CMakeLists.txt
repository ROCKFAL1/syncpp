cmake_minimum_required(VERSION 3.0.0)
project(storing_mutex_example)

add_executable(storing_mutex_example "main.cpp")

add_subdirectory("../../" "storing_mutex_example" EXCLUDE_FROM_ALL)
target_include_directories(storing_mutex_example PRIVATE ${SYNCPP_INCLUDE_DIRS})

set_target_properties(storing_mutex_example PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
)