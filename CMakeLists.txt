cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(heap_tracker VERSION 0.1 LANGUAGES CXX)

###############################################################################
# TODO: Use find_package or find_library
set(THIRD_PARTY_ROOT /home/arun/import)

# tcmalloc
set(TCMALLOC_ROOT ${THIRD_PARTY_ROOT}/gperftools)
include_directories(${TCMALLOC_ROOT}/src/)
link_directories(${TCMALLOC_ROOT}/.libs/)
###############################################################################

SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
# SET(CMAKE_CXX_COMPILER "/usr/bin/g++-6")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

include_directories(src)
add_subdirectory(src)


enable_testing()
add_subdirectory(test)
