#include <stdint.h>
void* get_base(uint64_t entry, uint64_t offset) { return (void*)(entry + offset); }
