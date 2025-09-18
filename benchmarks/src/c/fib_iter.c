// FILE: benchmarks/src/c/fib_iter.c
// Purpose: Iterative Fibonacci microbenchmark. Prints TIME_NS in nanoseconds.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#if defined(__APPLE__)
  #include <mach/mach_time.h>
  static inline long long now_ns(void) {
    static mach_timebase_info_data_t info = {0,0};
    if (info.denom == 0) mach_timebase_info(&info);
    // mach_absolute_time returns ticks; convert to ns
    return (long long)((__uint128_t)mach_absolute_time() * info.numer / info.denom);
  }
#else
  #include <time.h>
  static inline long long now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
  }
#endif

int main(int argc, char **argv) {
    int n = (argc > 1) ? atoi(argv[1]) : 90;

    long long t0 = now_ns();

    // Prevent trivial const-folding: keep values in 128-bit and branch on runtime n
    volatile unsigned __int128 a = 0, b = 1;
    for (int i = 0; i < n; i++) {
        unsigned __int128 t = a + b;
        a = b;
        b = t;
    }

    long long t1 = now_ns();
    printf("TASK=fib_iter,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    return 0;
}