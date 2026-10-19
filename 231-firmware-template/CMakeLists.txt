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

# железо прибора: единственное место проекта с SDK
add_library(platform STATIC
    platform/platform.c
)

target_include_directories(platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(platform PRIVATE
    pico_stdlib
)

# логика прибора: задачи и команды, без SDK
add_library(logic STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/api/api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/api/api-callbacks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/button-task/button-task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/led-task/led-task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/pi-task/pi-task.c
)

target_include_directories(logic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/logic
)

target_link_libraries(logic PRIVATE
    device
    logging
    profiling
    systime
PUBLIC
    stdio_text_protocol
)

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/logic
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    logging
    profiling
    stdio_text_protocol
    logic
    platform
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})