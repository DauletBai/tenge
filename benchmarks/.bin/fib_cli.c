#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
static inline long long now_ns(){ struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts); return (long long)ts.tv_sec*1000000000LL + (long long)ts.tv_nsec; }
int main(int argc,char**argv){
    int n    = (argc>1)?atoi(argv[1]):90;
    int reps = (argc>2)?atoi(argv[2]):1000000;
    volatile unsigned __int128 sink = 0;
    long long t0 = now_ns();
    for(int r=0; r<reps; r++){
        unsigned __int128 a=0,b=1;
        for(int i=0;i<n;i++){ unsigned __int128 t=a+b; a=b; b=t; }
        sink += b;
    }
    long long t1 = now_ns();
    long long avg_ns = (t1 - t0) / (reps>0?reps:1);
    printf("TASK=fib_iter,N=%d,TIME_NS=%lld\n", n, avg_ns);
    (void)sink; return 0;
}
