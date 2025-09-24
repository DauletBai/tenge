
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "runtime.h"    // now_ns()

int main(int argc, char** argv){
    int n = (argc>1)? atoi(argv[1]) : 100000;
    uint32_t* a = (uint32_t*)malloc(n*sizeof(uint32_t));
    uint32_t* b = (uint32_t*)malloc(n*sizeof(uint32_t));
    if(!a || !b){ fprintf(stderr,"oom\n"); return 1; }
    uint64_t x=88172645463393265ULL;
    for(int i=0;i<n;i++){ x = x*2862933555777941757ULL + 3037000493ULL; a[i] = (uint32_t)(x>>32); }
    long long t0 = now_ns();
    for(int pass=0; pass<4; pass++){
        unsigned cnt[256]={0};
        int shift = pass*8;
        for(int i=0;i<n;i++) cnt[(a[i]>>shift)&0xFF]++;
        unsigned sum=0;
        for(int i=0;i<256;i++){ unsigned c=cnt[i]; cnt[i]=sum; sum+=c; }
        for(int i=0;i<n;i++){ b[cnt[(a[i]>>shift)&0xFF]++] = a[i]; }
        uint32_t* tmp=a; a=b; b=tmp;
    }
    long long t1 = now_ns();
    printf("TASK=sort_radix,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    free(a); free(b);
    return 0;
}
