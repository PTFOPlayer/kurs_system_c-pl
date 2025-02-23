#include "../stdlib/stdint.h"
#include "../stdlib/string.h"
#include "../vga/vga.h"
#include "multiboot.h"

typedef struct mmap_entry {
    multiboot_uint32_t size;
    multiboot_uint32_t addr_low;
    multiboot_uint32_t addr_high;
    multiboot_uint32_t len_low;
    multiboot_uint32_t len_high;
#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5
    multiboot_uint32_t type;
} __attribute__((packed)) mmap_entry;

void init_memory(void *mbd, u8 debug) {
    char buff[32] = {0};
    memset(0, buff, 32);
    puts("Multiboot size:");
    u32 info_size = *(u32 *)mbd;
    itoa(info_size, buff, 10);
    puts(buff);
    puts("\n");

    if ((u64)mbd & 7) {
        puts("Unalighned mbd, panic");
        while (1) {
        }
    }
    
    u32 segment_size = 0;
    for (u32 i = 8; i < info_size;) {
        u32 type = *(u32 *)(mbd + i);
        u32 size = *(u32 *)(mbd + i + 4);

        puts("\tSegment type: ");
        memset(0, buff, 32);
        itoa(type, buff, 10);
        puts(buff);

        puts(" | Segment size: ");
        memset(0, buff, 32);
        itoa(size, buff, 10);
        puts(buff);
        puts("\n");
        i += (size + 7) & ~7;
    }

    // if(!(mbd->flags >> 6 & 0x1)) {
    //     puts("invalid memory map given by GRUB bootloader\n Hanging kernel");
    //     while (1){}
    // }

    // char buff[32] = {0};
    // memset(0, buff, 32);

    // if(debug) {
    //     puts("Multiboot info addr: 0x");
    //     itoa((i64)mbd, buff, 16);
    //     puts(buff);
    //     putc('\n');

    //     puts("\tMultiboot mmap addr: 0x");
    //     memset(0, buff, 32);
    //     itoa((i64)mbd->mmap_addr, buff, 16);
    //     puts(buff);
    //     putc('\n');

    //     puts("\tMultiboot mmap len: 0x");
    //     memset(0, buff, 32);
    //     itoa((i64)mbd->mmap_length, buff, 16);
    //     puts(buff);
    //     putc('\n');
    // }

    // for (
    //     i32 i = 0;
    //     i <  mbd->mmap_length;
    //     i += sizeof(multiboot_memory_map_t))
    // {

    //     multiboot_memory_map_t *entry =
    //     (multiboot_memory_map_t*)mbd->mmap_addr + i;
    //     // u64 entry_addr = (u64)(entry->addr_low) | ((u64)entry->addr_high
    //     << 32); i64 entry_addr = entry->addr; puts("\t\tEntry addr: 0x");
    //     memset(0, buff, 32);
    //     itoa(entry_addr, buff, 16);
    //     puts(buff);
    //     putc('\n');
    // }
}