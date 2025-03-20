#pragma once
#include "../../graphics/graphics.hpp"
#include "../../utils/utils.hpp"
#include "rsdt.hpp"

class XSDPHandle {
   private:
    XSDP* xsdp;
    XSDT* xsdt;

   public:
    XSDPHandle(XSDP*);

    void print();
    ~XSDPHandle() {};
};

XSDPHandle::XSDPHandle(XSDP* xsdp) {
    this->xsdp = xsdp;
    this->xsdt = (XSDT*)xsdp->XsdtAddress;
}

void XSDPHandle::print() {
    uint32_t entries = (xsdt->header.Length - sizeof(ACPISDTHeader)) / 8;
    for (size_t i = 0; i < entries; i++)
    {
        ACPISDTHeader *h= (ACPISDTHeader *)xsdt->other[i];
        char sig[5] = {0};    
        memcpy(sig, h->Signature, 4);   
        printf("\t%s\n", sig);
    }
    
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