cmake_minimum_required(VERSION 3.8)
project(Rini)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES ${MyProject_SOURCE_DIR}/src/rini.c ${MyProject_SOURCE_DIR}/src/rini.h ${MyProject_SOURCE_DIR}/src/lib.c ${MyProject_SOURCE_DIR}/src/lib.h)
add_library(Rini ${SOURCE_FILES})
