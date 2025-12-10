#pragma once

typedef struct Fiber {
    void* stack;
    void* context;
    void (*entry)(void*);
} Fiber;

typedef struct SwitchMessage {
    Fiber* fiber;
    void*  contexts;
} SwitchMessage;

// https://youtu.be/f30PceqQWko?t=417
SwitchMessage* ctx_switch(SwitchMessage* message) {
    // for x86_64
    asm("movq 0(%%rsi), %%rax\
         movq 8(%%rsi), %%rcx\
         leaq 0f(%%rip), %%rdx\
         movq %%rsp, 0(%%rax)\
         movq %%rbp, 8(%%rax)\
         movq %%rdx, 16(%%rax)\
         movq 0(%%rcx), %%rsp\
         movq 8(%%rcx), %%rbp\
         jmpq *16(%%rcx)\
         0:",
        [received_message] "={rsi}"(->* const @FieldType(SwitchMessage, "contexts")), //
        [message_to_send] "{rsi}"(&message.contexts),
        {"rax",   "rcx",   "rdx",   "rbx",   "rdi",   "r8",    "r9",    "r10",    "r11",    "r12",     "r13",   "r14",
         "r15",   "mm0",   "mm1",   "mm2",   "mm3",   "mm4",   "mm5",   "mm6",    "mm7",    "zmm0",    "zmm1",  "zmm2",
         "zmm3",  "zmm4",  "zmm5",  "zmm6",  "zmm7",  "zmm8",  "zmm9",  "zmm10",  "zmm11",  "zmm12",   "zmm13", "zmm14",
         "zmm15", "zmm16", "zmm17", "zmm18", "zmm19", "zmm20", "zmm21", "zmm22",  "zmm23",  "zmm24",   "zmm25", "zmm26",
         "zmm27", "zmm28", "zmm29", "zmm30", "zmm31", "fpsr",  "fpcr",  "mxcscr", "rflags", "dirflag", "memory"});
}

Fiber* fiber_new(void (*entry)(void*), void* context) {
    Fiber* fiber = malloc(sizeof(Fiber));
    fiber->entry = entry;
    fiber->context = context;
    return fiber;
}

void fiber_free(Fiber* fiber) { free(fiber); }

void fiber_switch(Fiber* fiber) {
    void* stack = fiber->stack;
    void* context = fiber->context;
    void (*entry)(void*) = fiber->entry;
    fiber->stack = stack;
    fiber->context = context;
    fiber->entry = entry;
    longjmp(context, 1);
}

void fiber_yield(Fiber* fiber) {
    void* stack = fiber->stack;
    void* context = fiber->context;
    void (*entry)(void*) = fiber->entry;
    fiber->stack = stack;
    fiber->context = context;
    fiber->entry = entry;
    longjmp(context, 2);
}

void fiber_run(Fiber* fiber) {
    void* stack = fiber->stack;
    void* context = fiber->context;
    void (*entry)(void*) = fiber->entry;
    fiber->stack = stack;
    fiber->context = context;
    fiber->entry = entry;
    setjmp(context);
    entry(context);
}
