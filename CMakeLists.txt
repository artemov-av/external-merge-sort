cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_CXX_STANDARD 17)
project(ExternalMergeSort)

set(SOURCES src/main.cpp src/sorter/sorter.h src/sorter/sorter.cpp src/utils/block_io.h)
include_directories("src/")
add_executable(ExternalMergeSort ${SOURCES})

