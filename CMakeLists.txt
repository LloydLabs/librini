cmake_minimum_required(VERSION 3.8)
project(Rini)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES rini.c rini.h lib.c lib.h)
add_library(Rini ${SOURCE_FILES})
