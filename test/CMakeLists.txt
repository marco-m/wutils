# Marco Molteni, February 2014.

# wutils/test

# Configuration file to silent the Wt logging.
configure_file(wt_config.xml wt_config.xml COPYONLY)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_executable(test-urlrouter test-urlrouter.cpp)
target_link_libraries(test-urlrouter
     ${GMOCK_LIBRARIES} ${Wt_LIBRARY} ${Wt_TEST_LIBRARY} ${Boost_LIBRARIES}
)
