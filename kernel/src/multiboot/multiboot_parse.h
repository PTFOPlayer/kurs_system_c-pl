#include "../stdlib/stdint.h"
#include "../stdlib/string.h"
#include "../vga/vga.h"
#include "multiboot.h"

char *segment_tags[] = {
    "End tag",
    "boot command line",
    "boot loader name",
    "modules",
    "basic memory information",
    "BIOS boot device",
    "memory map",
    "VBE info",
    "framebuffer info",
    "ELF-symbols",
    "APM table",
    "EFI 32-bit system table pointer",
    "EFI 64-bit system table pointer",
    "SMBIOS tables",
    "ACPI old RSDP",
    "ACPI new RSDP",
    "networking information",
    "EFI memory map",
    "EFI boot services not terminated",
    "EFI 32-bit image handle pointer",
    "EFI 64-bit image handle pointer",
    "image load base physical address",
};

void print_mmap(struct multiboot_tag_mmap *tag) {
    puts("\n\nMMAP\n");

    multiboot_memory_map_t *mmap;
    char buffer[32] = {0};
    for (mmap = tag->entries; (u8 *)mmap < (u8 *)tag + tag->size;
         mmap = (multiboot_memory_map_t *)((u64)mmap + tag->entry_size)) {
        printf("\t\tStart: 0x%x Len: %x\n", mmap->addr, mmap->len);
    }
};

typedef struct MultibootData {
    struct multiboot_tag_mmap *mmap_tag;
} MultibootData;

MultibootData parse_multiboot(void *mbd, u8 debug) {
    u32 info_size = *(u32 *)mbd;
    printf("Multiboot size: %d\n", info_size);

    if ((u64)mbd & 7) {
        set_color(Black, Red);
        puts("Unalighned mbd, panic");
        while (1) {
        }
    }

    MultibootData data;

    struct multiboot_tag *tag;
    for (u32 i = 8; i < info_size;) {
        tag = (struct multiboot_tag *)(mbd + i);
        u32 type = tag->type;
        u32 size = tag->size;

        printf("\t%s | size: %d\n", segment_tags[type], size);

        i += (size + 7) & ~7;

        if (type == MULTIBOOT_TAG_TYPE_MMAP) {
            data.mmap_tag = (struct multiboot_tag_mmap *)tag;
        }
    }
    return data;
}