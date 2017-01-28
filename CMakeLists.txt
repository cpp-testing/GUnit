#
# Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 2.8)
project(GUnit CXX)

option(ENABLE_MEMCHECK "Run the unit tests and examples under valgrind if it is found." OFF)

add_custom_target(style)
add_custom_command(TARGET style COMMAND find ${CMAKE_CURRENT_LIST_DIR}/benchmark ${CMAKE_CURRENT_LIST_DIR}/example ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/test -iname "*.h" -or -iname "*.cpp" | xargs clang-format -i)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")

enable_testing()

add_subdirectory(libs/googletest)

include_directories(include)
include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${gmock_SOURCE_DIR}/include)

set(BUILD_GMOCK)
set(BUILD_GTEST)

find_program(MEMORYCHECK_COMMAND valgrind)
if (ENABLE_MEMCHECK AND MEMORYCHECK_COMMAND)
    function(test name)
      string(REPLACE "/" "_" out ${name})
      add_executable(${out} ${CMAKE_CURRENT_LIST_DIR}/${name}.cpp)
      add_test(${out} ${MEMORYCHECK_COMMAND} --leak-check=full --error-exitcode=1 ./${out})
      target_link_libraries(${out} gtest_main)
      target_link_libraries(${out} gmock_main)
    endfunction()
else()
    function(test name)
      string(REPLACE "/" "_" out ${name})
      add_executable(${out} ${CMAKE_CURRENT_LIST_DIR}/${name}.cpp)
      add_test(${out} ./${out})
      target_link_libraries(${out} gtest_main)
      target_link_libraries(${out} gmock_main)
    endfunction()
endif()

test(test/GMake)
test(test/GMock)
test(test/GMockSyntax)
test(test/GTest)
test(example/GMock)
test(example/GTest)

include_directories(benchmark)
test(benchmark/GUnit/test)
test(benchmark/gtest/test)
