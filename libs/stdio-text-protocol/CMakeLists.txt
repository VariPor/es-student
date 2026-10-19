cmake_minimum_required(VERSION 3.15)

project(stdio_text_protocol)

add_library(${PROJECT_NAME} STATIC
    src/stdio-text-protocol.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_stdlib
PUBLIC
    logging
)