cmake_minimum_required(VERSION 3.14)
project(main)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-m32 -O2 -g")

include_directories(.)
include_directories(Common)

add_executable(main
	main.cc 
    testSquadGk.h
    lossycountwSgk.h
    lossycountwSgk.cc
    prng.cc
    rand48.cc
)