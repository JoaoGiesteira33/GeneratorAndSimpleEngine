cmake_minimum_required(VERSION 3.5)
project(Generator)

include_directories(
        ${PROJECT_SOURCE_DIR}/includes
        ${PROJECT_SOURCE_DIR}/src
)

file(GLOB all_SRCS
            "src/*.cpp"
            "includes/*.h"
            )
add_executable(Generator ${all_SRCS})
