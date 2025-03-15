#include <stdint.h>

#include "../graphics/graphics.hpp"
#include "../utils/utils.hpp"
#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

uint16_t pci_cfg_word(uint8_t bus, uint8_t device, uint8_t func,
                      uint8_t offset) {
    uint32_t lbus = ((uint32_t)bus) << 16;
    uint32_t ldevice = ((uint32_t)device) << 11;
    uint32_t lfunc = ((uint32_t)func) << 8;
    uint16_t tmp = 0;

    uint32_t address = (uint32_t)(lbus | ldevice | lfunc | (offset & 0xFC) |
                                  ((uint32_t)0x80000000));
    out_dw(0xCF8, address);
    tmp = (uint16_t)((in_dw(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

uint16_t vendor_id(uint8_t bus, uint8_t device, uint8_t func) {
    return pci_cfg_word(bus, device, func, 0);
}

uint16_t header_type(uint16_t bus, uint16_t device, uint16_t function) {
    return pci_cfg_word(bus, device, function, 0xE);
}

void check_device(uint8_t bus, uint8_t device) {
    uint8_t function = 0;

    uint16_t vendor = vendor_id(bus, device, function);
    if (vendor == 0xFFFF) return;

    uint16_t h_type = header_type(bus, device, function);
    if ((h_type & 0x80) == 0) {
        printf("\tBus: 0x%x, Device: 0x%x, Vendor: 0x%x, Header: %d\n", bus,
               device, vendor, h_type);
    } else {
        printf("\tBus: 0x%x, Device: 0x%x, Vendor: 0x%x, Multi Function\n", bus,
               device, vendor);
        for (function = 1; function < 8; function++) {
            if (vendor_id(bus, device, function) != 0xFFFF) {
                printf("\t\tFunction: %d, Header: %d\n", function,
                       header_type(bus, device, function));
            }
        }
    }
}

void check_pci() {
    uint16_t bus;
    uint8_t device;

    for (bus = 0; bus < 256; bus++) {
        for (device = 0; device < 32; device++) {
            check_device(bus, device);
        }
    }
}