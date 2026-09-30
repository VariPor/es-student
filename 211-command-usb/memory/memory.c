#include "memory.h"

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;

static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

void mem_info(void)
{
    // шапка таблицы: область, начало, конец, размер
    printf("%-10s %-10s %-10s %-8s\n", "area", "start", "end", "size");

    // flash — XIP_BASE и PICO_FLASH_SIZE_BYTES
    row("flash", (uintptr_t)XIP_BASE,
        (uintptr_t)(PICO_FLASH_SIZE_BYTES + XIP_BASE));
    // sram — базовый адрес из SDK, размер из datasheet
    row("sram", (uintptr_t)SRAM_BASE, (uintptr_t)(SRAM_END));
    // rom — базовый адрес из SDK, размер из datasheet
    const uint rom_size = 16 * 1024;
    row("rom", (uintptr_t)ROM_BASE, (uintptr_t)(ROM_BASE + rom_size));

    // image — от __flash_binary_start до __flash_binary_end
    row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);
    // free  — от __flash_binary_end до конца флеш-памяти
    row("free", (uintptr_t)&__flash_binary_end, (uintptr_t)(SRAM_END));
    // boot2 — от __boot2_start__ до __boot2_end__
    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);
    // text  — от __boot2_end__ до __etext: код и константы
    row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);

    // data flash — хранение .data, от __etext, длиной с .data
    row("data flash", (uintptr_t)&__etext, (uintptr_t)(&__etext + (&__data_end__ - &__data_start__)));
    // data ram   — работа .data, от __data_start__ до __data_end__
    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
    // bss        — от __bss_start__ до __bss_end__
    row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);
    // heap       — от __bss_end__ до __HeapLimit
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);
    // stack      — от __StackBottom до __StackTop
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);

    printf("\ntotal\n");
    // итог: образ во флеш и из чего он сложился
    const uint boot2_size = &__boot2_end__ - &__boot2_start__;
    const uint text_size = &__etext - &__boot2_end__;
    const uint data_size = &__data_end__ - &__data_start__;
    const uint flash_used = boot2_size + text_size + data_size;
    printf("  flash image  %8u = boot2 %u + text %u + data %u\n",
           flash_used, boot2_size, text_size, data_size);
    // итог: свободно во флеш-памяти из всего её объёма
    printf("  flash free  %8u of %u\n", PICO_FLASH_SIZE_BYTES - flash_used,
           PICO_FLASH_SIZE_BYTES);
    // итог: занято в ОЗУ — .data и .bss
    const uint bss_size = &__bss_end__ - &__bss_start__;
    const uint ram_used = data_size + bss_size;
    printf("  ram used    %8u = data %u + bss %u\n", ram_used, data_size, bss_size);
    // итог: свободно в ОЗУ — под кучу и под стек
    const uint heap_free = &__HeapLimit - &__bss_end__;
    const uint stack_free = &__StackTop - &__StackBottom;
    printf("  ram free    %8u for heap and %u for stack\n", heap_free, stack_free);
}