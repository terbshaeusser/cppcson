cmake_minimum_required(VERSION 3.10)
project(example)

set(CMAKE_CXX_STANDARD 11)

# Disable building cppcson tests
set(CPPCSON_BUILD_TESTS OFF)

# Add cppcson as sub directory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/cppcson)

# Define our executable and link to cppcson
add_executable(example
        main.cpp
        )

target_link_libraries(example cppcson)
