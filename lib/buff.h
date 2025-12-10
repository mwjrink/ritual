#pragma once

#include <arena.h>
#include <bp.h>
#include <string.h>

// TODO Temp
#include <logger.h>

// 16 bytes
typedef struct AllocBuff {
    void* data;
    usize len;
} Buff;

typedef struct ContainedBuff {
    usize len;
    u8    data[];
} HBuff;

// TODO small buffer opt with a union?
// 20 bytes
typedef struct AllocDynList {
    usize cap;
    usize len;
    void* data;

    usize element_size;
} AllocDynList;

#define DEFAULT_CAP 1024 // TODO this is huge

AllocDynList alloc_list_create(Arena* arena, usize element_size) {
    void* data = arena_alloc(arena, DEFAULT_CAP * element_size);

    return (AllocDynList){
        .cap = DEFAULT_CAP,
        .len = 0,
        .data = data,
        .element_size = element_size,
    };
}

AllocDynList alloc_list_create_with_cap(Arena* arena, usize element_size, usize cap) {
    void* data = arena_alloc(arena, cap * element_size);

    return (AllocDynList){
        .cap = cap,
        .len = 0,
        .data = data,
        .element_size = element_size,
    };
}

void alloc_list_push(Arena* arena, AllocDynList* list, void* element) {
    if (list->len == list->cap) {
        // TODO realloc/expand
    }

    memcpy(list->data + list->len * list->element_size, element, list->element_size);
    list->len += 1;
}

void alloc_list_push_ptr(Arena* arena, AllocDynList* list, void* element) {
    assert(list->element_size == sizeof(void*));
    if (list->len == list->cap) {
        // TODO realloc/expand
    }

    memcpy(list->data + list->len * list->element_size, &element, list->element_size);
    list->len += 1;
}
