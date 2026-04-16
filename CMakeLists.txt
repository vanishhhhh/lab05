cmake_minimum_required(VERSION 3.10)

project(lab05 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)

add_subdirectory(banking)

if(BUILD_TESTS)
  enable_testing()

  add_subdirectory(third-party/gtest)

  file(GLOB LAB05_TEST_SOURCES tests/*.cpp)

  add_executable(check ${LAB05_TEST_SOURCES})
  target_link_libraries(check banking gtest_main)

  add_test(NAME check COMMAND check)
endif()