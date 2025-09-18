#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static inline long long now_ns(){struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return (long long)ts.tv_sec*1000000000LL+ts.tv_nsec;}
static int cmpi(const void*a,const void*b){int ia=*(const int*)a, ib=*(const int*)b; return (ia>ib)-(ia<ib);}
int main(int argc,char**argv){int n=(argc>1)?atoi(argv[1]):100000; int*arr=(int*)malloc(sizeof(int)*n); for(int i=0;i<n;i++){arr[i]=n-i;} long long t0=now_ns(); qsort(arr,n,sizeof(int),cmpi); long long t1=now_ns(); printf("TASK=sort_qsort,N=%d,TIME_NS=%lld\n",n,(t1-t0)); free(arr); return 0;}
