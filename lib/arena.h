#pragma once

// #ifndef _ARENA_IMPL_H
// #define _ARENA_IMPL_H

#include <bp.h>
#include <string.h>
#include <sys/mman.h> /* mmap() is defined in this header */

// #include <unistd.h>

#define PAGE_SIZE (1024 * 1024 * 1024)

// ARG ORDER IS DIFFERENT ON LINUX

void* alloc_page() {
    void* data = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); //
    madvise(data, PAGE_SIZE, MADV_HUGEPAGE);
    // THE ADV CONFLICT WITH EACH-OTHER // madvise(data, PAGE_SIZE, MADV_HUGEPAGE | MADV_WILLNEED | MADV_SEQUENTIAL);
    return data;
}

// TODO dump assets into .o files and link them? Ryan Fleury mentioned this in wookash vs Ginger Bill
// better way of embedding assets into an executable

// 20 bytes
typedef struct Arena {
    u8*   data;
    usize len;
    u8*   ckpt; // use a u32 index instead of a pointer?
} Arena;

// FUTURE allow custom page size.
//     This requires custom checks in alloc etc for size/cap
Arena arena_create() {
    u8* data = alloc_page();

    // posix_memalign();

    // instead of mmap?
    // void* current_ptr = sbrk(0);
    // void* new_mem = sbrk(100);
    // if (new_mem == (void*)-1) {
    //    // FAIL
    // }
    // brk(current_ptr); // recall to a previous block

    Arena arena = {
        .ckpt = 0,
        .data = (data + sizeof(Arena)),
        .len = 0,
    };

    *(Arena*)data = arena;

    return arena;
}

// checkpoint
// TODO there is a bug if you have full or nearly full arena and try to make a checkpoint.
// Also check and silently disallow consecutive checkpoints??
// Actually... I should let them. It keeps the code consistent for start and finish and runtime branching
void arena_ckpt(Arena* arena) {
    u8* new_ckpt = arena->data + arena->len;
    *(u8**)new_ckpt = arena->ckpt;
    arena->len += sizeof(arena->ckpt);
    arena->ckpt = new_ckpt;
}

void arena_rollback(Arena* arena) {
    if (arena->ckpt == 0) {
        Arena old_page = *(Arena*)(arena->data - sizeof(Arena));

        if (arena->data == old_page.data) {
            *arena = old_page;
            return;
        }

        munmap(arena->data - sizeof(Arena), PAGE_SIZE);
        *arena = old_page;
        return;
    }

    arena->len = sizeof(u8*) + arena->ckpt - arena->data;
}

// TODO do I really want this to potentially be slow and unmap a page?
void arena_pop(Arena* arena) {
    if (arena->ckpt == 0) {
        Arena old_page = *(Arena*)(arena->data - sizeof(Arena));

        if (arena->data == old_page.data) {
            *arena = old_page;
            return;
        }

        munmap(arena->data - sizeof(Arena), PAGE_SIZE);
        *arena = old_page;
        return;
    }

    arena->len = arena->ckpt - arena->data;
    arena->ckpt = *(u8**)arena->ckpt;
}

void arena_force_new_page(Arena* arena) {
    u8* new_page = alloc_page();
    // printf("new page created: %p\n", new_page);

    *(Arena*)new_page = *arena;
    arena->data = new_page + sizeof(Arena);
    arena->len = 0;
    arena->ckpt = 0;
}

void* arena_alloc(Arena* arena, usize size) {
    assert(size < PAGE_SIZE);
    if (size + arena->len > PAGE_SIZE - (sizeof(Arena))) {
        arena_force_new_page(arena);
    }

    u8* result = arena->data + arena->len;
    arena->len += size;
    return result;
}

// TODO do I really want this to potentially be slow and allocate a new page?
void* arena_alloc_fail_on_expand(Arena* arena, usize size) {
    assert(size < PAGE_SIZE);
    if (size + arena->len > PAGE_SIZE - (sizeof(Arena))) {
        return NULL;
    }

    u8* result = arena->data + arena->len;
    arena->len += size;
    return result;
}

// return the data pointer and do nothing.
// If you call anything else on arena after
// calling this and before calling post, UB
void* arena_pre_alloc(Arena* arena) { return arena->data + arena->len; }

// After calling pre and writing some data to the arena,
// tell me how much data you wrote.
void arena_post_alloc(Arena* arena, usize size) {
    assert(size < PAGE_SIZE - arena->len);
    arena->len += size;
}

char* arena_copy_str(Arena* arena, char* str) {
    void* dst = arena->data + arena->len;
    u8*   new_loc = memccpy(dst, str, '\0', PAGE_SIZE - arena->len);
    arena->len = new_loc - arena->data;
    return dst;
}

#ifdef TEST

#include <stdio.h>
#include <unistd.h> // Required for sleep()
void arena_test() {

    printf("Starting Arena Alloc Tests!\n");
    Arena arena = arena_create();

    printf("first page: %p\n", arena.data);

    printf("Allocating 200 x 4096 * 1024\n");
    u8* final;
    for (int i = 0; i < 200; i++) {
        final = (u8*)arena_alloc(&arena, 4096 * 1024);
        *final = 0xFF;
    }

    printf("Creating Checkpoint\n");
    arena_ckpt(&arena);

    printf("Allocating 55 x 4096\n");
    for (int i = 0; i < 55; i++) {
        final = (u8*)arena_alloc(&arena, 4096 * 1024);
        *final = 0xFF;
    }

    printf("Rolling Back\n");
    arena_rollback(&arena);

    printf("Allocating 55 x 4096\n");
    for (int i = 0; i < 55; i++) {
        final = (u8*)arena_alloc(&arena, 4096 * 1024);
        *final = 0xFF;
    }

    printf("Creating Checkpoint\n");
    arena_ckpt(&arena);

    printf("Writing to final ptr\n");
    for (int i = 0; i < 4096; i++) {
        final[i] = 0xFF;
    }

    printf("Allocating 1 x 4096 * 20\n");
    u8* new_page = arena_alloc(&arena, 4096 * 1024 * 20);

    for (int i = 0; i < 10000; i++) {
        final = (u8*)arena_alloc(&arena, 4096 * 1024 * 25);
        *final = 0xFF;
    }
    sleep(5); // Pause execution for 5 seconds

    printf("second page: %p\n", arena.data);

    printf("Writing to new_page ptr\n");
    for (int i = 0; i < 40960; i++) {
        new_page[i] = 0xFF;
    }

    printf("Creating Checkpoint\n");
    arena_ckpt(&arena);

    printf("Allocating 3 x 40960\n");
    new_page = arena_alloc(&arena, 4096 * 1024 * 10);
    *new_page = 0xFF;
    new_page = arena_alloc(&arena, 4096 * 1024 * 10);
    *new_page = 0xFF;
    new_page = arena_alloc(&arena, 4096 * 1024 * 10);
    *new_page = 0xFF;

    printf("Rolling Back\n");
    arena_rollback(&arena);

    printf("Rolling Back to Previous Page\n");
    arena_rollback(&arena);

    printf("Rolling Back 5 times\n");
    printf("on page: %p\n", arena.data);
    printf("1\n");
    arena_rollback(&arena);
    printf("on page: %p\n", arena.data);
    printf("2\n");
    arena_rollback(&arena);
    printf("on page: %p\n", arena.data);
    printf("3\n");
    arena_rollback(&arena);
    printf("on page: %p\n", arena.data);
    printf("4\n");
    arena_rollback(&arena);
    printf("on page: %p\n", arena.data);
    printf("5\n");
    arena_rollback(&arena);
    printf("on page: %p\n", arena.data);

    printf("Done!\n");
}

#endif

// #endif
