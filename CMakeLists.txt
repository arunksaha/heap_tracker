cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(heap_tracker VERSION 0.2 LANGUAGES CXX)

###############################################################################

# Exactly one of the two options must be ON.
option(BUILD_INTERPOSITION "Build interposition based interception" OFF)
option(BUILD_TCMALLOC      "Build interposition based interception" OFF)

message("BUILD_INTERPOSITION: ${BUILD_INTERPOSITION}")
message("BUILD_TCMALLOC: ${BUILD_TCMALLOC}")

if (BUILD_INTERPOSITION)
  add_definitions(-DHEAP_TRACKER_INTERCEPT_INTERPOSITION)
elseif (BUILD_TCMALLOC)
  add_definitions(-DHEAP_TRACKER_INTERCEPT_TCMALLOC)
else (BUILD_INTERPOSITION)
  message(FATAL_ERROR "Exactly one interception policy between "
    " BUILD_INTERPOSITION and BUILD_TCMALLOC must be speficied.")
endif ()

###############################################################################

# As an option, clang can be used for building. If unspeficied, default is gcc.
option(BUILD_CLANG "Build with clang" OFF)

if (BUILD_CLANG)
  SET(CMAKE_CXX_COMPILER "/usr/bin/clang++-5.0")
  message("Building with clang")
else (BUILD_CLANG)
  # gcc is the default option.
  SET(CMAKE_CXX_COMPILER "/usr/bin/g++-7")
  message("Building with gcc")
endif (BUILD_CLANG)
message("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

###############################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
# The following two lines are for backtrace.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


include_directories(src)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)
