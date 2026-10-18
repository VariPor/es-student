cmake_minimum_required(VERSION 3.15)

project(libs)

add_library(${PROJECT_NAME})

add_subdirectory(device)
add_subdirectory(logging)
add_subdirectory(profiling)
add_subdirectory(systime)
add_subdirectory(stdio-text-protocol)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    device
    logging
    profiling
    stdio_text_protocol
    systime
)

