cmake_minimum_required(VERSION 3.15)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include(pico_sdk_import.cmake)

project(231_firmware_template)

pico_sdk_init()

add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)
target_compile_definitions(profiling PUBLIC
    PROFILING_ENABLED=0
)
add_compile_options(-Wall)

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/api/api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/led-task/led-task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/button-task/button-task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pi-task/pi-task.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/led-task
    ${CMAKE_CURRENT_SOURCE_DIR}/button-task
    ${CMAKE_CURRENT_SOURCE_DIR}/pi-task
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    device
    logging
    profiling
    stdio_text_protocol
    systime
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})