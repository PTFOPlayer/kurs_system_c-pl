#include <stdint.h>

#include "../interrupts/interrupts.hpp"
#include "../memory/ll_allocator.hpp"
class Context {
   private:
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rsp, rbp, rdx, rcx,
        rbx, rax;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;

    void *stack;
    uint64_t stack_size;
    uint64_t pid = 0;
    char process_name[128] = {0};

   public:
    static uint64_t next_pid;
    Context(uint32_t, void (*)());
    Context(uint32_t, void (*)(), char *);
    ~Context();
    void copy_frame(IRQFrame *);
    void set_frame(IRQFrame *);
};

uint64_t Context::next_pid = 0;

Context::Context(uint32_t stack_size, void (*context)()) {
    this->pid = this->next_pid;
    this->stack_size = stack_size;
    this->stack = malloc(stack_size);
    this->rsp = (uint64_t)this->stack;
    rip = (uint64_t)context;
    next_pid++;
}

Context::Context(uint32_t stack_size, void (*context)(), char *name) {
    *this = Context(stack_size, context);

    for (uint32_t i = 0; name[i] != '\0' && i < 127; i++) {
        this->process_name[i] = name[i];
    }
}

Context::~Context() {}

void Context::copy_frame(IRQFrame *frame) {
    this->r15 = frame->r15;
    this->r14 = frame->r14;
    this->r13 = frame->r13;
    this->r12 = frame->r12;
    this->r11 = frame->r11;
    this->r10 = frame->r10;
    this->r9 = frame->r9;
    this->r8 = frame->r8;
    this->rdi = frame->rdi;
    this->rsi = frame->rsi;
    this->rsp = frame->rsp;
    this->rbp = frame->rbp;
    this->rdx = frame->rdx;
    this->rcx = frame->rcx;
    this->rbx = frame->rbx;
    this->rax = frame->rax;
    this->rip = frame->rip;
    this->cs = frame->cs;
    this->rflags = frame->rflags;
}

void Context::set_frame(IRQFrame *frame) {
    frame->r15 = this->r15;
    frame->r14 = this->r14;
    frame->r13 = this->r13;
    frame->r12 = this->r12;
    frame->r11 = this->r11;
    frame->r10 = this->r10;
    frame->r9 = this->r9;
    frame->r8 = this->r8;
    frame->rdi = this->rdi;
    frame->rsi = this->rsi;
    frame->rsp = this->rsp;
    frame->rbp = this->rbp;
    frame->rdx = this->rdx;
    frame->rcx = this->rcx;
    frame->rbx = this->rbx;
    frame->rax = this->rax;
    frame->rip = this->rip;
    frame->cs = this->cs;
    frame->rflags = this->rflags;
}