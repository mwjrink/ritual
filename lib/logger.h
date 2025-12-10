#pragma once

#include <bp.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __x86_64
#include <x86intrin.h>
#define get_clocks() __rdtsc()
#elif defined(__aarch64__)
/*
 * According to ARM DDI 0487F.c, from Armv8.0 to Armv8.5 inclusive, the
 * system counter is at least 56 bits wide; from Armv8.6, the counter
 * must be 64 bits wide.  So the system counter could be less than 64
 * bits wide and it is attributed with the flag 'cap_user_time_short'
 * is true.
 */
#define get_clocks()                                                                                                   \
    ({                                                                                                                 \
        u64 val;                                                                                                       \
        asm volatile("mrs %0, cntvct_el0" : "=r"(val));                                                                \
        (unsigned long long)val;                                                                                       \
    })

#else
#include <time.h>
// cast for compat with __rdtsc()
#define get_clocks() (unsigned long long)clock()
#endif

typedef enum Severity {
    VERBOSE,
    INFO,
    DEBUG,
    WARNING,
    ERROR,
    CRITICAL,
} Severity;

#define A_START       "\e["
#define A_BOLD        "1"
#define A_FG(r, g, b) "38;2;" #r ";" #g ";" #b
#define A_BG(r, g, b) "48;2;" #r ";" #g ";" #b
#define A_RESET       "\e[0m"
#define A_END         "m"

// printf(ANSI_ESCAPE_START ANSI_BOLD ANSI_COLOR_BACKGROUND(255, 127, 127) ANSI_ESCAPE_END
// "Background Red\n" ANSI_RESET);
// printf(ANSI_ESCAPE_START ANSI_BOLD ANSI_COLOR_FOREGROUND(0, 255, 127) ANSI_COLOR_BACKGROUND(255, 127, 127)
// ANSI_ESCAPE_END "Background Red & Foreground Green\n" ANSI_RESET);

// printf("\e[1;38;2;23;147;209mCachyOS \e[0mBlue\n");
// printf("\e[1;48;2;23;147;209mCachyOS \e[0mBlue\n");
// sleep(5);

void _log(const Severity severity, char* file, int line, char* fmt, ...) {
    printf(A_START);

    va_list args;
    va_start(args, fmt);

    switch (severity) {
        case VERBOSE: {
#ifdef LOG_LEVEL_VERBOSE
            printf(A_BOLD ";" A_BG(64, 64, 64) ";" A_FG(200, 200, 200));
            printf(A_END);
            printf(" VERBOSE  " A_RESET);
#endif
        } break;
        case INFO: {
#ifdef LOG_LEVEL_INFO
            printf(A_BOLD ";" A_BG(5, 200, 255) ";" A_FG(255, 255, 255));
            printf(A_END);
            printf(" INFO     " A_RESET);
#endif
        } break;
        case DEBUG: {
#ifdef LOG_LEVEL_DEBUG
            printf(A_BOLD ";" A_BG(50, 50, 255) ";" A_FG(255, 255, 255));
            printf(A_END);
            printf(" DEBUG    " A_RESET);
#endif
        } break;
        case WARNING: {
#ifdef LOG_LEVEL_WARNING
            printf(A_BOLD ";" A_BG(200, 150, 0) ";" A_FG(255, 255, 255));
            printf(A_END);
            printf(" WARNING  " A_RESET);
#endif
        } break;
        case ERROR: {
#ifdef LOG_LEVEL_ERROR
            printf(A_BOLD ";" A_BG(200, 0, 0) ";" A_FG(255, 255, 255) A_END);
            printf(" ERROR    " A_RESET);
#endif
        } break;
        case CRITICAL: {
#ifdef LOG_LEVEL_CRITICAL
            printf(A_BOLD ";" A_BG(255, 255, 255) ";" A_FG(200, 0, 0));
            printf(A_END);
            printf(" CRITICAL " A_RESET);
#endif
        } break;
    }

    printf(" | " A_START A_BG(160, 124, 196) ";" A_FG(31, 12, 0) ";" A_BOLD A_END " %s:%i " A_RESET " | ", file, line);

#ifdef linux
#define _GNU_SOURCE
#include <sys/syscall.h>
#include <unistd.h>
    long thread_id = syscall(SYS_gettid);
    // pid_t thread_id = gettid();
#elif defined(win32)
    DWORD thread_id = GetCurrentThreadId();
#endif

#ifdef __x86_64
    u32                core_id = -1;
    unsigned long long timestamp = __rdtscp(&core_id);
    printf(A_START A_BG(255, 130, 218) ";" A_FG(31, 12, 0) ";" A_BOLD A_END " %llu clocks " A_RESET " | ", timestamp);
    printf(A_START A_BG(50, 168, 82) ";" A_FG(31, 12, 0) ";" A_BOLD A_END " THREAD: %ld on CORE: %u" A_RESET " | ",
           thread_id,
           core_id);
#elif defined(__aarch64__)
/*
 * According to ARM DDI 0487F.c, from Armv8.0 to Armv8.5 inclusive, the
 * system counter is at least 56 bits wide; from Armv8.6, the counter
 * must be 64 bits wide.  So the system counter could be less than 64
 * bits wide and it is attributed with the flag 'cap_user_time_short'
 * is true.
 */
#define get_clocks()                                                                                                   \
    u64 val;                                                                                                           \
    asm volatile("mrs %0, cntvct_el0" : "=r"(val));

    printf(A_START A_BG(255, 130, 218) ";" A_FG(31, 12, 0) ";" A_BOLD A_END " %llu clocks " A_RESET " | ",
           (unsigned long long)val);
#else
#include <time.h>
    // cast for compat with __rdtsc()
    printf(A_START A_BG(255, 130, 218) ";" A_FG(31, 12, 0) ";" A_BOLD A_END " %llu clocks " A_RESET " | ",
           (unsigned long long)clock());
#endif

    vprintf(fmt, args);

    va_end(args);

    printf("\n");
}

// __DATE__ for compile date, __TIME__ for compile time
// __STDC_VERSION__ for c strandard version

#define VERBOSE_LOG(...)  _log(VERBOSE, __FILE__, __LINE__, __VA_ARGS__)
#define INFO_LOG(...)     _log(INFO, __FILE__, __LINE__, __VA_ARGS__)
#define DEBUG_LOG(...)    _log(DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define WARNING_LOG(...)  _log(WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define ERROR_LOG(...)    _log(ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define CRITICAL_LOG(...) _log(CRITICAL, __FILE__, __LINE__, __VA_ARGS__)
