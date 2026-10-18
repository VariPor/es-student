cmake_minimum_required(VERSION 3.15)

project(logging)

add_library(${PROJECT_NAME} STATIC
    src/log.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

