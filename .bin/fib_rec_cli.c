
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "runtime.h"    // now_ns()

static unsigned long long fib_rec_c(int n){
    return (n<=1)? (unsigned long long)n : fib_rec_c(n-1)+fib_rec_c(n-2);
}
int main(int argc, char** argv){
    int n = (argc>1)? atoi(argv[1]) : 35;
    long long t0 = now_ns();
    volatile unsigned long long r = fib_rec_c(n);
    long long t1 = now_ns();
    (void)r;
    printf("TASK=fib_rec,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    return 0;
}
