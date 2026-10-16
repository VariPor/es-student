#pragma once

#include "pico/unique_id.h"
#include "pico/version.h"

#define DEVICE_NAME "es-firmare-template"
#define FIRMWARE_VERSION "1.2.0"

#define DEVICE_PROJECT "231-firmware-template"
#define DEVICE_REPO "https://github.com/VariPor/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

#include <stdint.h>

struct info_t
{
    uint32_t version;
    char name[13];
    uint8_t revision;
};

struct device_info_t
{
    char device_board[8];                                        // тип платы
    char serial_number[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1]; // серийный номер
    unsigned int manufacturer;                                   // производитель
    unsigned int part;                                           // изделие кристалла
    unsigned int revision;                                       // ревизия кристалла
};

void device_get_info(struct device_info_t *info);
