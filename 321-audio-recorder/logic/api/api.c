#include "api/api.h"
#include "api/api-commands.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "platform/platform.h"

void api_handle(const command_t *command)
{
    if (command->truncated)
    {
        printf("error: command longer than 63 characters or 4 arguments\n");
        return;
    }
    if (strcmp(command->name, "help") == 0)
    {
        printf("%-20s list of commands\n", "help");
        for (uint32_t i = 0; i < api_command_count; ++i)
        {
            printf("%-20s %s\n", api_commands[i].name, api_commands[i].help);
        }
        return;
    }
    for (uint32_t i = 0; i < api_command_count; ++i)
    {
        if (strcmp(api_commands[i].name, command->name) == 0)
        {
            api_commands[i].callback(command);
            return;
        }
    }
    printf("error: unknown command '%s', try help\n", command->name);
}

void api_send_frame(const stream_header_t *header, const uint8_t *data, uint32_t bytes)
{
    // байты заголовка и отсчётов как они лежат в памяти
    platform_write(header, sizeof(*header));
    platform_write(data, bytes);
}
