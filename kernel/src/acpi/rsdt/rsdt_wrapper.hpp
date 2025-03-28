#pragma once
#include "../../graphics/graphics.hpp"
#include "../../utils/utils.hpp"
#include "../fadt/fadt.hpp"
#include "rsdt.hpp"

class XSDPHandle {
   private:
    XSDP* xsdp;
    XSDT* xsdt;

   public:
    XSDPHandle(XSDP*);

    void print();
    FADT* find_fadt();
    ~XSDPHandle() {};
};

XSDPHandle::XSDPHandle(XSDP* xsdp) {
    this->xsdp = xsdp;
    this->xsdt = (XSDT*)xsdp->XsdtAddress;
}

void XSDPHandle::print() {
    uint32_t entries = (xsdt->header.Length - sizeof(ACPISDTHeader)) / 8;
    for (size_t i = 0; i < entries; i++) {
        ACPISDTHeader* h = (ACPISDTHeader*)xsdt->other[i];
        char sig[5] = {0};
        memcpy(sig, h->Signature, 4);
        printf("\t%s\n", sig);
    }
}

FADT* XSDPHandle::find_fadt() {
    uint32_t entries = (xsdt->header.Length - sizeof(ACPISDTHeader)) / 8;
    for (size_t i = 0; i < entries; i++) {
        ACPISDTHeader* h = (ACPISDTHeader*)xsdt->other[i];
        if (h->Signature[0] == 'F' && h->Signature[1] == 'A' &&
            h->Signature[2] == 'D' && h->Signature[3] == 'T') {
            return (FADT*)h;
        }
    }
    return nullptr;
}

class RSDPWrapper {
   private:
    XSDP* xsdp;
    bool is_20 = false;

   public:
    RSDPWrapper(void* rsdp);
    XSDPHandle get_xsdp();
    bool is_xsdp();

    ~RSDPWrapper();
};

RSDPWrapper::RSDPWrapper(void* rsdp) {
    this->xsdp = (XSDP*)rsdp;
    this->is_20 = this->xsdp->rsdp.Revision == 2;
}

XSDPHandle RSDPWrapper::get_xsdp() { return XSDPHandle(this->xsdp); }
bool RSDPWrapper::is_xsdp() { return is_20; }
RSDPWrapper::~RSDPWrapper() {}