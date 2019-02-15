cmake_minimum_required(VERSION 3.13)
project(httpserver C)
aux_source_directory(. DIRSRCS)
set(CMAKE_C_STANDARD 99)

add_executable(httpserver ${DIRSRCS})