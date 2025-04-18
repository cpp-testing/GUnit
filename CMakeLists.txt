#
# Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.12)
project(GUnit CXX)

set(MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
endif()

option(GUNIT_ENABLE_MEMCHECK "Run the unit tests and examples under valgrind if it is found." OFF)
option(GUNIT_ENABLE_COVERAGE "Run coverage." OFF)
option(GUNIT_BUILD_BENCHMARKS "Build the benchmarks" ${MASTER_PROJECT})
option(GUNIT_BUILD_EXAMPLES "Build the examples" ${MASTER_PROJECT})
option(GUNIT_BUILD_TESTS "Build the tests" ${MASTER_PROJECT})

add_custom_target(style)
add_custom_command(TARGET style COMMAND find ${CMAKE_CURRENT_LIST_DIR}/benchmark ${CMAKE_CURRENT_LIST_DIR}/example ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/test -iname "*.h" -or -iname "*.cpp" | xargs clang-format -i)

set(CMAKE_CXX_STANDARD 17)

if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")
endif()

if(MSVC)
  add_compile_options(/EHsc)
  # Statically link libraries
  # See https://gitlab.kitware.com/cmake/cmake/-/issues/18390
  add_compile_options(
          $<$<CONFIG:>:/MT>
          $<$<CONFIG:Debug>:/MTd>
          $<$<CONFIG:Release>:/MT>
  )
endif(MSVC)

if(ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
endif()

enable_testing()

add_subdirectory(libs/googletest)

add_library(gunit INTERFACE)
target_include_directories(gunit INTERFACE include)
target_include_directories(gunit
  INTERFACE ${gtest_SOURCE_DIR}/include
  ${gmock_SOURCE_DIR}/include
  libs/json/single_include/nlohmann
)

target_link_libraries(gunit
  INTERFACE gtest_main
  INTERFACE gmock_main
  INTERFACE gherkin-cpp
)

set(BUILD_GMOCK)
set(BUILD_GTEST)

add_subdirectory(libs/gherkin-cpp)

find_program(MEMORYCHECK_COMMAND valgrind)
if (ENABLE_MEMCHECK AND MEMORYCHECK_COMMAND)
  function(test name scenario)
    string(REPLACE "/" "_" out ${name})
    add_executable(${out} ${CMAKE_CURRENT_LIST_DIR}/${name}.cpp)
    add_test(${out} ${MEMORYCHECK_COMMAND} --leak-check=full --error-exitcode=1 ./${out})
    set_tests_properties(${out}
    PROPERTIES
    ENVIRONMENT "${scenario};TEST_NAME=${out}"
    )
    target_link_libraries(${out} gunit)
  endfunction()
else()
  function(test name scenario)
    string(REPLACE "/" "_" out ${name})
    add_executable(${out} ${CMAKE_CURRENT_LIST_DIR}/${name}.cpp)
    add_test(${out} ./${out})
    set_tests_properties(${out}
    PROPERTIES
    ENVIRONMENT "${scenario};TEST_NAME=${out}"
    )
    target_link_libraries(${out} gunit)
  endfunction()
endif()

if(GUNIT_BUILD_EXAMPLES)
  test(example/GAssert SCENARIO=)
  test(example/GMock SCENARIO=)
  test(example/GTest SCENARIO=)
  test(example/GTest-Lite SCENARIO=)
  test(example/GSteps SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/example/GSteps.feature)
endif()

if(GUNIT_BUILD_TESTS)
  include_directories(test)
  test(test/GAssert SCENARIO=)
  test(test/GMake SCENARIO=)
  test(test/GMock SCENARIO=)
  test(test/GSteps SCENARIO=)
  test(test/Features/Repeat/Steps/RepeatSteps SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/test/Features/Repeat/repeat.feature)
  test(test/Features/Calc/Steps/CalcSteps SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/test/Features/Calc/addition.feature:${CMAKE_CURRENT_SOURCE_DIR}/test/Features/Calc/additionfile2.feature:${CMAKE_CURRENT_SOURCE_DIR}/test/Features/Calc/division.feature)
  test(test/Features/Data/Steps/DataSteps SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/test/Features/Data/data.feature)
  #test(test/Features/Error/Steps/ErrorSteps SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/test/Features/Calc/addition.feature:${CMAKE_CURRENT_SOURCE_DIR}/test/Features/Error/error.feature)
  test(test/Features/Table/Steps/TableSteps SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/test/Features/Table/table.feature)
  test(test/Features/Tags/Steps/TagsSteps SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/test/Features/Tags/tags.feature)
  test(test/GTest SCENARIO=)
  test(test/GTest-Lite SCENARIO=)
  test(test/Detail/FileUtils SCENARIO=)
  test(test/Detail/Preprocessor SCENARIO=)
  test(test/Detail/ProgUtils SCENARIO=)
  test(test/Detail/RegexUtils SCENARIO=)
  test(test/Detail/StringUtils SCENARIO=)
  test(test/Detail/TypeTraits SCENARIO=)
  test(test/Detail/Utility SCENARIO=)
endif()

if(GUNIT_BUILD_BENCHMARKS)
  include_directories(benchmark)
  test(benchmark/GUnit/test SCENARIO=)
  test(benchmark/gtest/test SCENARIO=)
endif()
