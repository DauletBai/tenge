#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static inline long long now_ns(){struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return (long long)ts.tv_sec*1000000000LL+ts.tv_nsec;}
long long fib(int n){return n<2?n:fib(n-1)+fib(n-2);}
int main(int argc,char**argv){int n=(argc>1)?atoi(argv[1]):35; long long t0=now_ns(); volatile long long r=fib(n); long long t1=now_ns(); (void)r; printf("TASK=fib_rec,N=%d,TIME_NS=%lld\n",n,(t1-t0)); return 0;}
