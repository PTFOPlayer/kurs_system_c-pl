#pragma once
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

struct multiboot_elf_sections_entry {
    u32 name;
    u32 type;
    u64 flags;
    u64 addr;
    u64 offset;
    u64 size;
    u32 link;
    u32 info;
    u64 addralign;
    u64 entsize;
} __attribute__((packed));

void print_kernel_sections(struct multiboot_tag_elf_sections *tag) {
    printf("\n\nELF Kernel Sections\n");

    struct multiboot_elf_sections_entry *section =
        (struct multiboot_elf_sections_entry *)(tag->sections);

    u64 kernel_start_addr = __UINT64_MAX__;
    u64 kernel_end_addr = 0;
    for (u32 i = 0; i < tag->num; i++) {
        printf("\t\tAddr: 0x%x, Size: %x, Type: %d, Flags: 0x%x\n",
               section[i].addr, section[i].size, section[i].type,
               section[i].flags);
        if (section[i].addr != 0 && section[i].addr < kernel_start_addr)
            kernel_start_addr = section[i].addr;
        if (section[i].addr + section[i].size > kernel_end_addr)
            kernel_end_addr = section[i].addr + section[i].size;
    }

    printf("Kernel start: 0x%x, Kernel end: 0x%x", kernel_start_addr,
           kernel_end_addr);
}

#define MAX_MEMORY_AREAS 128

typedef struct MultibootData {
    u64 multiboot_information_start;
    u64 multiboot_information_end;
    struct mmap {
        multiboot_memory_map_t *mmap;
        u32 mmap_size;
    } mmap;
    struct kernel_addr {
        u64 start;
        u64 end;
    } kernel_addr;

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
    data.multiboot_information_start = (u64)mbd;
    data.multiboot_information_end = (u64)mbd + info_size;

    struct multiboot_tag *tag;
    for (u32 i = 8; i < info_size;) {
        tag = (struct multiboot_tag *)(mbd + i);
        u32 type = tag->type;
        u32 size = tag->size;

        printf("\t%s | size: %d\n", segment_tags[type], size);

        i += (size + 7) & ~7;

        switch (type) {
            case MULTIBOOT_TAG_TYPE_MMAP:
                struct multiboot_tag_mmap *mmap_tag =
                    (struct multiboot_tag_mmap *)tag;

                u64 entry_size = mmap_tag->entry_size;
                multiboot_memory_map_t *mmap;
                data.mmap.mmap = mmap;

                for (mmap = mmap_tag->entries;
                     (u8 *)mmap < (u8 *)mmap_tag + mmap_tag->size;
                     mmap = (multiboot_memory_map_t *)(mmap + entry_size)) {
                    data.mmap.mmap_size++;
                }

                break;
            case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
                struct multiboot_tag_elf_sections *sections_tag =
                    (struct multiboot_tag_elf_sections *)tag;

                struct multiboot_elf_sections_entry *section =
                    (struct multiboot_elf_sections_entry *)(sections_tag
                                                                ->sections);
                u64 kernel_start_addr = __UINT64_MAX__;
                u64 kernel_end_addr = 0;
                for (u32 i = 0; i < sections_tag->num; i++) {
                    if (section[i].addr != 0 &&
                        section[i].addr < kernel_start_addr)
                        kernel_start_addr = section[i].addr;
                    if (section[i].addr + section[i].size > kernel_end_addr)
                        kernel_end_addr = section[i].addr + section[i].size;
                }

                data.kernel_addr.start = kernel_start_addr;
                data.kernel_addr.end = kernel_end_addr;
            default:
                break;
        }
    }
    return data;
}

void print_mmap(struct MultibootData *data) {
    printf("\n\nMMAP\n");

    for (u32 i = 0; i < data->mmap.mmap_size; i++) {
        printf("\t\tStart: 0x%x Len: %x Type: %d\n", data->mmap.mmap[i].addr,
               data->mmap.mmap[i].len, data->mmap.mmap[i].type);
    }
};