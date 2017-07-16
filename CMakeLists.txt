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
include_directories(libs/json/src)
include_directories(libs/gherkin-cpp/include)

link_directories(${CMAKE_CURRENT_LIST_DIR}/libs/gherkin-cpp)

set(BUILD_GMOCK)
set(BUILD_GTEST)

add_custom_command(
  OUTPUT libgherkin-cpp
  COMMAND cd ../libs/gherkin-cpp && make
)

add_custom_target(
  gherkin_cpp ALL DEPENDS libgherkin-cpp
)

find_program(MEMORYCHECK_COMMAND valgrind)
if (ENABLE_MEMCHECK AND MEMORYCHECK_COMMAND)
    function(test name)
      string(REPLACE "/" "_" out ${name})
      add_executable(${out} ${CMAKE_CURRENT_LIST_DIR}/${name}.cpp)
      add_test(${out} ${MEMORYCHECK_COMMAND} --leak-check=full --error-exitcode=1 ./${out})
      add_dependencies(${out} gherkin_cpp)
      target_link_libraries(${out} gtest_main gmock_main gherkin-cpp)
    endfunction()
else()
    function(test name)
      string(REPLACE "/" "_" out ${name})
      add_executable(${out} ${CMAKE_CURRENT_LIST_DIR}/${name}.cpp)
      add_test(${out} ./${out})
      add_dependencies(${out} gherkin_cpp)
      target_link_libraries(${out} gtest_main gmock_main gherkin-cpp)
    endfunction()
endif()

test(example/GMock)
test(example/GTest)
test(example/GScenario)

include_directories(test)
test(test/GMake)
test(test/GMock)
test(test/GScenario)
test(test/Features/Calc/Steps/CalcSteps)
test(test/Features/Table/Steps/TableSteps)
test(test/Features/Tags/Steps/TagsSteps)
test(test/GTest)
test(test/GTest-Lite)
test(test/Detail/FileUtils)
test(test/Detail/Preprocessor)
test(test/Detail/ProgUtils)
test(test/Detail/RegexUtils)
test(test/Detail/StringUtils)
test(test/Detail/TypeTraits)
test(test/Detail/Utility)

include_directories(benchmark)
test(benchmark/GUnit/test)
test(benchmark/gtest/test)
