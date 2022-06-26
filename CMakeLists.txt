cmake_minimum_required(VERSION 3.17)
project(lab_14)

set(CMAKE_CXX_STANDARD 17)
include_directories(include/)

add_executable(lab_14 include/linq.hpp src/smoke_test.cpp)