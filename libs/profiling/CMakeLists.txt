cmake_minimum_required(VERSION 3.15)

project(profiling)

add_library(${PROJECT_NAME} STATIC
    src/profiling.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    systime
)
