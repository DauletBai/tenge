
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "runtime.h"    // now_ns()

int main(int argc, char** argv){
    int n = (argc>1)? atoi(argv[1]) : 90;
    long long t0 = now_ns();
    __uint128_t a=0,b=1;
    for(int i=0;i<n;i++){ __uint128_t t=a+b; a=b; b=t; }
    long long t1 = now_ns();
    printf("TASK=fib_iter,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    return 0;
}
