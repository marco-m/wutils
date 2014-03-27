# Marco Molteni, March 2014.

project(wutils CXX)
cmake_minimum_required(VERSION 2.8.11)
message(STATUS "Entering project ${PROJECT_NAME}")

########### BEGIN User-settable configuration ############

set(WT_DIR    "" CACHE PATH "Where to find Wt")
set(GMOCK_DIR "" CACHE PATH "Where to find GoogleMock")

########### END User-settable configuration ##############

list(APPEND CMAKE_INCLUDE_PATH ${WT_DIR} ${GMOCK_DIR})
list(APPEND CMAKE_LIBRARY_PATH ${WT_DIR} ${GMOCK_DIR})

# Where to pick up our finders.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(GNU_CXX_FLAGS
    " -W -Wall -Wextra -Wno-unused-parameter"
    " -O0 -g -pipe -std=c++11"
)
string(REPLACE ";" "" GNU_CXX_FLAGS ${GNU_CXX_FLAGS})

if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    set(CMAKE_CXX_FLAGS ${GNU_CXX_FLAGS})
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    # -stdlib=libc++ : use the clang stdlib
    set(CMAKE_CXX_FLAGS "${GNU_CXX_FLAGS} -stdlib=libc++")
    # -coverage
    #set(CMAKE_EXE_LINKER_FLAGS "--coverage")
endif()

#
# Find Boost (needed by Wt).
#
find_package(Boost 1.50.0 REQUIRED COMPONENTS signals system)
include_directories(${Boost_INCLUDE_DIRS})

#
# Find Wt.
#
find_package(Wt REQUIRED) # XXX Wt finder doesn't obey REQUIRED
if(NOT Wt_FOUND)
    message(FATAL_ERROR "Wt not found. Have you set WT_DIR ?")
endif()
include_directories(${Wt_INCLUDE_DIR})

#
# Find GMock and Gtest.
#
find_package(GMOCK)
if(NOT GMOCK_FOUND)
    message(FATAL_ERROR "GMOCK not found. Have you set GMOCK_DIR ?")
endif()
include_directories(${GMOCK_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})

#
# Build WUtils.
#
include_directories(${CMAKE_SOURCE_DIR})

add_library(wutils
    urlrouter.cpp
)

#
# Build tests.
#
add_subdirectory(test)

message(STATUS "Exiting project ${PROJECT_NAME}")
