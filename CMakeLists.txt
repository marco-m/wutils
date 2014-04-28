# Marco Molteni, March 2014.

project(wutils CXX)
cmake_minimum_required(VERSION 2.8.7) # Travis supports only this old cmake
message(STATUS "Entering project ${PROJECT_NAME}")
message(STATUS "Using cmake version ${CMAKE_VERSION}")
message(STATUS "Running on ${CMAKE_HOST_SYSTEM} ${CMAKE_HOST_SYSTEM_PROCESSOR}"
    " ${CMAKE_LIBRARY_ARCHITECTURE}")

########### BEGIN User-settable configuration ############

set(WT_DIR    "" CACHE PATH "Where to find Wt")
set(GMOCK_DIR "" CACHE PATH "Where to find GoogleMock")

########### END User-settable configuration ##############

list(APPEND CMAKE_INCLUDE_PATH ${WT_DIR} ${GMOCK_DIR})
list(APPEND CMAKE_LIBRARY_PATH ${WT_DIR} ${GMOCK_DIR})

# Where to pick up our finders.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

#
# Check if the compiler supports C++11 and fail if not.
#
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CXX11_FLAG "-std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    message(WARNING "C++ compiler too old. Compilation will fail.")
    set(CXX11_FLAG "-std=c++0x")
else()
    message(FATAL_ERROR "The C++ compiler has no C++11 support.")
endif()

set(GNU_CXX_FLAGS
    " -W -Wall -Wextra -Wno-unused-parameter"
    " -O0 -g -pipe ${CXX11_FLAG}"
)
string(REPLACE ";" "" GNU_CXX_FLAGS ${GNU_CXX_FLAGS})

if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${GNU_CXX_FLAGS}")
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    set(CMAKE_CXX_FLAGS "${GNU_CXX_FLAGS} -stdlib=libc++")
endif()

# -coverage
#set(CMAKE_EXE_LINKER_FLAGS "--coverage")



#
# Find Boost (needed by Wt).
find_package(Boost 1.54.0 REQUIRED COMPONENTS signals system regex)
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
    message(FATAL_ERROR "GMOCK not found. You have two options:\n"
        "1. From the build directory, run the script get-gmock.sh\n"
        "2. Install gmock by hand\n"
        "Then, in both cases, set GMOCK_DIR")
endif()
include_directories(${GMOCK_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})

#
# Build WUtils.
#
include_directories(${CMAKE_SOURCE_DIR})

#add_library(wutils
#)

# Show the include files in an IDE.
add_custom_target(dummy SOURCES src/urlrouter.hpp)

#
# Build tests.
#
add_subdirectory(test)

message(STATUS "Exiting project ${PROJECT_NAME}")

# http://stackoverflow.com/questions/8454329/why-cant-clang-with-libc-in-c0x-mode-link-this-boostprogram-options-examp
#
# You need to rebuild boost using clang++ -stdlib=libc++.
#
# libc++ is not binary compatible with gcc's libstdc++ (except for some low level stuff such as operator new).
# For example the std::string in gcc's libstdc++ is refcounted, whereas in libc++ it uses the
# "short string optimization". If you were to accidentally mix these two strings in the same program (and mistake
# them for the same data structure), you would inevitably get a run time crash.
#
# In order to turn this run time crash into a link time error, libc++ uses a C++11 language feature called
# inline namespace to change the ABI of std::string without impacting the API of std::string. That is, to you
# std::string looks the same. But to the linker, std::string is being mangled as if it is in namespace std::__1.
# Thus the linker knows that std::basic_string and std::__1::basic_string are two different data structures
# (the former coming from gcc's libstdc++ and the latter coming from libc++).
#

