cmake_minimum_required(VERSION 3.16)
project(muduo)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC")

aux_source_directory(src SOURCES)

add_library(muduo SHARED ${SOURCES})