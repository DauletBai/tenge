#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
static inline long long now_us(){ struct timeval tv; gettimeofday(&tv, NULL); return (long long)tv.tv_sec*1000000LL + (long long)tv.tv_usec; }
int main(int argc,char**argv){
    int n    = (argc>1)?atoi(argv[1]):90;
    int reps = (argc>2)?atoi(argv[2]):1000000;
    volatile unsigned __int128 sink = 0;
    long long t0 = now_us();
    for(int r=0; r<reps; r++){
        unsigned __int128 a=0,b=1;
        for(int i=0;i<n;i++){ unsigned __int128 t=a+b; a=b; b=t; }
        sink += b;
    }
    long long t1 = now_us();
    long long avg_ns = ((t1 - t0) * 1000LL) / (reps>0?reps:1);
    printf("TASK=fib_iter,N=%d,TIME_NS=%lld\n", n, avg_ns);
    (void)sink; return 0;
}
