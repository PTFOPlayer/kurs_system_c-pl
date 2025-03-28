#pragma once
#include <stdint.h>
#include "../acpi.hpp"
struct RSDP {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__((packed));

struct XSDP {
    RSDP rsdp;
    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t ExtendedChecksum;
    uint8_t reserved[3];
} __attribute__((packed));

struct RSDT {
    ACPISDTHeader header;
    uint32_t other[];
} __attribute__((packed));

struct XSDT {
    ACPISDTHeader header;
    uint64_t other[];
} __attribute__((packed));
