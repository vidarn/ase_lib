cmake_minimum_required (VERSION 2.6)
project (ASE_Loader_Example)

include_directories("../include")
set(CMAKE_C_FLAGS "-std=c99 -Wall -pedantic") 

add_executable(example main.c)
