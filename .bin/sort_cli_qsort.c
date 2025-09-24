
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "runtime.h"    // now_ns()

static int cmp_int(const void* a, const void* b){
    int x = *(const int*)a, y = *(const int*)b;
    return (x>y)-(x<y);
}
int main(int argc, char** argv){
    int n = (argc>1)? atoi(argv[1]) : 100000;
    int* a = (int*)malloc(n*sizeof(int));
    if(!a){ fprintf(stderr,"oom\n"); return 1; }
    uint64_t x=88172645463393265ULL;
    for(int i=0;i<n;i++){ x = x*2862933555777941757ULL + 3037000493ULL; a[i] = (int)(x>>33); }
    long long t0 = now_ns();
    qsort(a,n,sizeof(int),cmp_int);
    long long t1 = now_ns();
    printf("TASK=sort_qsort,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    free(a);
    return 0;
}
