#pragma once
// #ifndef _BOILERPLATE_H
// #define _BOILERPLATE_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
// typedef uint128_t u128;
// typedef uintptr_t ptr;
// typedef uintptr_t usize;
// apparently using u32 as idx is faster even on 64 bit system?
// typedef uint32_t usize;
typedef size_t usize;
typedef u32    uidx;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float  f32;
typedef double f64;

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// #endif
