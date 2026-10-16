#include "device.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "hardware/regs/sysinfo.h"
#include "hardware/regs/addressmap.h"

void device_info(struct device_info_t *info)
{
       pico_get_unique_board_id_string(info->serial_number, sizeof(info->serial_number));
       strcpy(info->device_board, DEVICE_BOARD);

       volatile unsigned int *chip_id = (unsigned int *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
       unsigned int id = (unsigned int)(*chip_id);
       info->manufacturer = (unsigned int)((id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >>
                                           SYSINFO_CHIP_ID_MANUFACTURER_LSB);

       info->part = (unsigned int)((id & SYSINFO_CHIP_ID_PART_BITS) >>
                                   SYSINFO_CHIP_ID_PART_LSB);

       info->revision = (unsigned int)((id & SYSINFO_CHIP_ID_REVISION_BITS) >>
                                       SYSINFO_CHIP_ID_REVISION_LSB);
       /*printf("decice name: %s\n", DEVICE_NAME);
       printf("firmware version: %s\n", FIRMWARE_VERSION);
       printf("project: %s\n", DEVICE_PROJECT);
       printf("repo: %s\n", DEVICE_REPO);
       printf("pico-sdk: %s\n", PICO_SDK_VERSION_STRING);*/
}
