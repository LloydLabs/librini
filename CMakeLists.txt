cmake_minimum_required(VERSION 2.8)
project(Rini)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES src/rini.c src/rini.h src/lib.c src/lib.h)
add_library(Rini ${SOURCE_FILES})
