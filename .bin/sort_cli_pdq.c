
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "runtime.h"    // now_ns()

static inline void iswap(int* a, int* b){ int t=*a; *a=*b; *b=t; }
static int median3(int* a,int i,int j,int k){
    int x=a[i], y=a[j], z=a[k];
    if((x<y) ^ (x<z)) return i;
    if((y<x) ^ (y<z)) return j;
    return k;
}
static int part(int* a,int l,int r){
    int m = l + (r-l)/2;
    int p = median3(a,l,m,r);
    iswap(&a[p], &a[r]);
    int pivot = a[r], i=l;
    for(int j=l;j<r;j++) if(a[j] <= pivot){ iswap(&a[i], &a[j]); i++; }
    iswap(&a[i], &a[r]);
    return i;
}
static void insertion(int* a,int l,int r){
    for(int i=l+1;i<=r;i++){
        int x=a[i], j=i-1;
        while(j>=l && a[j]>x){ a[j+1]=a[j]; j--; }
        a[j+1]=x;
    }
}
static void introsort_rec(int* a,int l,int r,int depth){
    while(r-l>32){
        if(depth==0){
            for(int i=l;i<=r;i++){
                int mi=i;
                for(int j=i+1;j<=r;j++) if(a[j]<a[mi]) mi=j;
                iswap(&a[i],&a[mi]);
            }
            return;
        }
        int p = part(a,l,r);
        if(p-l < r-p){ introsort_rec(a,l,p-1,depth-1); l=p+1; }
        else         { introsort_rec(a,p+1,r,depth-1); r=p-1; }
    }
    insertion(a,l,r);
}
int main(int argc, char** argv){
    int n = (argc>1)? atoi(argv[1]) : 100000;
    int* a = (int*)malloc(n*sizeof(int));
    if(!a){ fprintf(stderr,"oom\n"); return 1; }
    uint64_t x=88172645463393265ULL;
    for(int i=0;i<n;i++){ x = x*2862933555777941757ULL + 3037000493ULL; a[i] = (int)(x>>33); }
    long long t0 = now_ns();
    int depth = 2; while((1<<depth) < n) depth++;
    introsort_rec(a,0,n-1,depth*2);
    long long t1 = now_ns();
    printf("TASK=sort_pdq,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    free(a);
    return 0;
}
