cmake_minimum_required(VERSION 3.9)
project (ink-cpp-test CXX)
add_executable (ink-cpp-test test/main.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.)
target_compile_features(ink-cpp-test PUBLIC cxx_std_11)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(ink-cpp-test pthread)
endif()