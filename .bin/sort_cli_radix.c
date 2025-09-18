#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
static inline long long now_ns(){struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return (long long)ts.tv_sec*1000000000LL+ts.tv_nsec;}
static void radix_u32(uint32_t* a,uint32_t* tmp,int n){
    const int K=256;
    int cnt[K];
    for(int pass=0; pass<4; pass++){
        memset(cnt,0,sizeof(cnt));
        int shift = pass*8;
        for(int i=0;i<n;i++){ cnt[(a[i]>>shift)&0xFF]++; }
        int sum=0; for(int i=0;i<K;i++){ int c=cnt[i]; cnt[i]=sum; sum+=c; }
        for(int i=0;i<n;i++){ tmp[ cnt[(a[i]>>shift)&0xFF]++ ] = a[i]; }
        memcpy(a,tmp,sizeof(uint32_t)*n);
    }
}
int main(int argc,char**argv){
    int n=(argc>1)?atoi(argv[1]):100000;
    uint32_t* arr=(uint32_t*)malloc(sizeof(uint32_t)*n);
    uint32_t* tmp=(uint32_t*)malloc(sizeof(uint32_t)*n);
    for(int i=0;i<n;i++){ arr[i]=(uint32_t)(n-i); }
    long long t0=now_ns();
    radix_u32(arr,tmp,n);
    long long t1=now_ns();
    printf("TASK=sort_radix,N=%d,TIME_NS=%lld\n",n,(t1-t0));
    free(arr); free(tmp);
    return 0;
}
