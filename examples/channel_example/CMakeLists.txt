cmake_minimum_required(VERSION 3.0.0)
project(channel_example)

add_executable(channel_example "main.cpp")

add_subdirectory("../../" "channel_example" EXCLUDE_FROM_ALL)
target_include_directories(channel_example PRIVATE ${SYNCPP_INCLUDE_DIRS})

set_target_properties(channel_example PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
)