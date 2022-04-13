cmake_minimum_required(VERSION 3.6.0)
project(syncpp)

set(SYNCPP_SOURCES
    "include/syncpp/rw_lock.hpp"
    "include/syncpp/storing_mutex.hpp"
    "include/syncpp/channel.hpp"
)

set(SYNCPP_INCLUDE_DIRS
     ${CMAKE_CURRENT_LIST_DIR}/include
      CACHE PATH "Syncpp include dirs")

add_library(syncpp STATIC ${SYNCPP_SOURCES})

set_target_properties(syncpp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
)

