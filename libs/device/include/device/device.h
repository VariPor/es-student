#pragma once

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
    char device_board[8];      // тип платы
    char serial_number[17];    // серийный номер
    unsigned int manufacturer; // производитель
    unsigned int part;         // изделие кристалла
    unsigned int revision;     // ревизия кристалла
    char pico_sdk_version[12];
};

void device_get_info(struct device_info_t *info);
