cmake_minimum_required(VERSION 3.0.0)
project(rw_lock_example)

add_executable(rw_lock_example "main.cpp")

add_subdirectory("../../" "rw_lock_example" EXCLUDE_FROM_ALL)
target_include_directories(rw_lock_example PRIVATE ${SYNCPP_INCLUDE_DIRS})

set_target_properties(rw_lock_example PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
)