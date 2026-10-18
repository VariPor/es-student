cmake_minimum_required(VERSION 3.15)

project(device)

add_library(${PROJECT_NAME} STATIC
    src/device.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_unique_id
)


target_compile_definitions(${PROJECT_NAME} PRIVATE
    DEVICE_BOARD="${PICO_BOARD}"
)

