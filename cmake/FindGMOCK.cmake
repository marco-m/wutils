# Find Google Mock and the bundled Google Test
# Marco Molteni, March 2014.

# GMOCK_FOUND
# GMOCK_INCLUDE_DIRS
# GMOCK_LIBRARIES
#
# GTEST_FOUND
# GTEST_INCLUDE_DIRS
# GTEST_LIBRARIES

find_path(GMOCK_INCLUDE_DIR gmock/gmock.h PATH_SUFFIXES include)
find_path(GTEST_INCLUDE_DIR gtest/gtest.h PATH_SUFFIXES include)

find_library(GMOCK_LIBRARY gmock PATH_SUFFIXES lib)
find_library(GMOCK_MAIN_LIBRARY gmock_main PATH_SUFFIXES lib)
find_library(GTEST_LIBRARY gtest PATH_SUFFIXES lib)
find_library(GTEST_MAIN_LIBRARY gtest_main PATH_SUFFIXES lib)

set(GMOCK_LIBRARIES ${GMOCK_LIBRARY} ${GMOCK_MAIN_LIBRARY})
set(GMOCK_INCLUDE_DIRS ${GMOCK_INCLUDE_DIR})

set(GTEST_LIBRARIES ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMOCK DEFAULT_MSG
        GMOCK_LIBRARY GMOCK_MAIN_LIBRARY GMOCK_INCLUDE_DIR)
find_package_handle_standard_args(GTEST DEFAULT_MSG
        GTEST_LIBRARY GTEST_MAIN_LIBRARY GTEST_INCLUDE_DIR)
