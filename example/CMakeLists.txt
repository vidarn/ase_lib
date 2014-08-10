cmake_minimum_required (VERSION 2.6)
project (ASE_Loader_Example)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(ICU)

include_directories(
    "../include"
    ${ICU_INCLUDE_DIRS}
)
set(CMAKE_C_FLAGS "-Wall -pedantic") 

add_executable(example main.c)
target_link_libraries(example ${ICU_LIBRARIES})
