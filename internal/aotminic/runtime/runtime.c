// FILE: internal/aotminic/runtime/runtime.c
#include "runtime.h"

#if defined(__APPLE__)
/* macOS / iOS: mach_absolute_time */
#include <mach/mach_time.h>

long long now_ns(void) {
    static mach_timebase_info_data_t tb = {0, 0};
    if (tb.denom == 0) mach_timebase_info(&tb);
    uint64_t t = mach_absolute_time();
    /* convert to ns */
    return (long long)((t * tb.numer) / tb.denom);
}

#else
/* POSIX: clock_gettime(CLOCK_MONOTONIC) */
#include <time.h>

long long now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + (long long)ts.tv_nsec;
}
#endif