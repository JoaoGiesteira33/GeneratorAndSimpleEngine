cmake_minimum_required(VERSION 3.5)
project(Engine)

include_directories(
        ${PROJECT_SOURCE_DIR}/includes
        ${PROJECT_SOURCE_DIR}/src
)

file(GLOB all_SRCS 
            "src/*.cpp"
            "include/*.h"
            main.cpp)
add_executable(Engine ${all_SRCS})