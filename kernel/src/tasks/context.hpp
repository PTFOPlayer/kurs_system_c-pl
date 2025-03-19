#include <stdint.h>

#include "../interrupts/interrupts.hpp"
#include "../memory/ll_allocator.hpp"

struct Context {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t rflags;
    uint64_t rip;
} __attribute__((packed));

void setup_context(struct Context *ctx, void (*entry)()) {
    void *stack = malloc(4096);

    ctx->rsp = (uint64_t)(stack + 4096 - 8);
    ctx->rbp = ctx->rsp;
    ctx->rip = (uint64_t)entry;

    asm volatile(
        "pushfq\n"
        "movq (%%rsp), %%rax\n"
        "movq %%rax, %0\n"
        "popfq\n"
        : "=m"(ctx->rflags)::"%rax");

    *(uint64_t *)(ctx->rsp) = 0;
}

extern "C" void context_switch(Context *old_ctx, Context *new_ctx);

void store_irq_context(Context *ctx, IRQFrame *irq) {
    ctx->rax = irq->rax;
    ctx->rbx = irq->rbx;
    ctx->rcx = irq->rcx;
    ctx->rdx = irq->rdx;
    ctx->rsi = irq->rsi;
    ctx->rdi = irq->rdi;
    ctx->rsp = irq->rsp;
    ctx->rbp = irq->rbp;
    ctx->r8 = irq->r8;
    ctx->r9 = irq->r9;
    ctx->r10 = irq->r10;
    ctx->r11 = irq->r11;
    ctx->r12 = irq->r12;
    ctx->r13 = irq->r13;
    ctx->r14 = irq->r14;
    ctx->r15 = irq->r15;
    ctx->rflags = irq->rflags;
    ctx->rip = irq->rip;
}
