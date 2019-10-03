cmake_minimum_required(VERSION 3.10)
project(cppcson)

option(CPPCSON_BUILD_TESTS "Build cppcson tests" ON)

add_library(cppcson
        Include/cppcson.hpp
        Source/cppcson.cpp
        )

target_include_directories(cppcson PUBLIC Include)

target_compile_features(cppcson PUBLIC cxx_std_11)

target_compile_options(cppcson PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -pedantic -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /WX>
        )

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
