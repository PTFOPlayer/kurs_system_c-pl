#include <stdint.h>

#include "../interrupts/interrupts.hpp"
#include "../memory/ll_allocator.hpp"
class Context {
   private:
    IRQFrame frame;
    uint64_t pid = 0;
    char process_name[128] = {0};

    void (*handle)();

    static const uint32_t stack_size = 16384;
    void *stack_ptr;

   public:
    static uint64_t next_pid;

    Context(void (*)());
    void set_name(char *);
    ~Context();
    void prepare_from_frame(IRQFrame *);
    void copy_frame(IRQFrame *);
    void set_frame(IRQFrame *);
    void restore_return(IRQFrame *);
};

uint64_t Context::next_pid = 0;

Context::Context(void (*handle)()) {
    this->pid = this->next_pid;
    this->handle = handle;
    // + 128 "protection buffer"
    this->stack_ptr = malloc(this->stack_size + 128);
    this->stack_ptr = (void *)(((uintptr_t)this->stack_ptr + 15) & ~15);

    next_pid++;
}

void Context::set_name(char *name) {
    for (uint32_t i = 0; name[i] != '\0' && i < 127; i++) {
        this->process_name[i] = name[i];
    }
}

void Context::prepare_from_frame(IRQFrame *f) {
    f->rip = (uint64_t)this->handle;
    f->stack =
        (uint64_t)this->stack_ptr + this->stack_size - (5 * sizeof(uint64_t));
    f->optional_rsp = (uint64_t)this->stack_ptr + this->stack_size;
    uint64_t *temp_stack = (uint64_t *)f->stack;
    temp_stack[0] = f->rip;
    temp_stack[1] = f->cs;
    temp_stack[2] = f->rflags;
    temp_stack[3] = f->optional_rsp;
    temp_stack[4] = f->optional_ss;

    this->frame = *f;
}

Context::~Context() {}

void Context::copy_frame(IRQFrame *f) { this->frame = *f; }

void Context::set_frame(IRQFrame *f) { *f = this->frame; }

void Context::restore_return(IRQFrame *f) {
    printf("rip: %x, opt rsp: %x, rsp: %x\n",f->rip, f->optional_rsp, f->stack); 

    uint64_t *temp_stack = (uint64_t*)this->frame.stack;
    temp_stack[0] = f->rip;
    temp_stack[1] = f->cs;
    temp_stack[2] = f->rflags;
    temp_stack[3] = f->optional_rsp;
    temp_stack[4] = f->optional_ss;
    
    f->stack = (uint64_t)temp_stack;
}