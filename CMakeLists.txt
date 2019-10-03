cmake_minimum_required(VERSION 3.10)
project(cppcson)

option(CPPCSON_BUILD_TESTS "Build cppcson tests" ON)

set(CMAKE_CXX_STANDARD 11)

add_library(cppcson
        Include/cppcson.hpp
        Source/cppcson.cpp
        )

target_include_directories(cppcson PUBLIC Include)

if (${CPPCSON_BUILD_TESTS})
    find_package(GTest REQUIRED)
    find_package(Threads REQUIRED)

    enable_testing()

    add_executable(csontests
            Tests/csontests.cpp
            )

    target_include_directories(csontests PRIVATE ${GTEST_INCLUDE_DIRS})
    target_link_libraries(csontests ${GTEST_BOTH_LIBRARIES} Threads::Threads cppcson)

    add_test(NAME csontests COMMAND csontests)
endif ()
