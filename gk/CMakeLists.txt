cmake_minimum_required(VERSION 3.14)
project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O2 -g")

include_directories(.)
include_directories(Common)

add_executable(main main_gk.cpp Common/BOBHash.h)
