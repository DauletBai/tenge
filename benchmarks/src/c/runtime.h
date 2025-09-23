#ifndef TENGE_BENCH_RUNTIME_H
#define TENGE_BENCH_RUNTIME_H

// Minimal local runtime helpers for C benchmarks.
// Provides: now_ns() and a lightweight PRNG.

#include <stdint.h>

#if defined(__APPLE__)
  #include <mach/mach_time.h>
  static inline long long now_ns(void) {
    static mach_timebase_info_data_t info = {0,0};
    if (info.denom == 0) mach_timebase_info(&info);
    uint64_t t = mach_absolute_time();
    return (long long)((t * info.numer) / info.denom);
  }
#else
  #include <time.h>
  static inline long long now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
  }
#endif

// SplitMix64 for quick deterministic integers
static inline uint64_t splitmix64(uint64_t *x) {
  uint64_t z = (*x += 0x9e3779b97f4a7c15ULL);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  return z ^ (z >> 31);
}

// 31-bit non-negative int
static inline int rand_i32(uint64_t *state) {
  return (int)(splitmix64(state) & 0x7fffffff);
}

#endif // TENGE_BENCH_RUNTIME_H