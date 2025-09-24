
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "runtime.h"    // now_ns()

static inline uint64_t xorshift64(uint64_t* s){
    uint64_t x=*s; x^=x>>12; x^=x<<25; x^=x>>27; *s=x; return x*0x2545F4914F6CDD1DULL;
}
static inline double u01(uint64_t* s){ return (xorshift64(s)>>11)*(1.0/9007199254740992.0); }

int main(int argc, char** argv){
    int N    = (argc>1)? atoi(argv[1]) : 1000000;
    int steps= (argc>2)? atoi(argv[2]) : 1;
    double a = (argc>3)? atof(argv[3]) : 0.99;
    double* pnl = (double*)malloc(N*sizeof(double));
    if(!pnl){ fprintf(stderr,"oom\n"); return 1; }
    long long t0 = now_ns();
    uint64_t s=1234567;
    for(int i=0;i<N;i++){
        double x=0.0;
        for(int k=0;k<steps;k++){
            double u=u01(&s), v=u01(&s);
            double r=sqrt(-2.0*log(u+1e-18));
            double z=r*cos(6.283185307179586*v);
            x += z;
        }
        pnl[i] = x;
    }
    int idx = (int)((1.0-a)*N); if(idx<0) idx=0; if(idx>=N) idx=N-1;
    int l=0, r=N-1;
    while(l<r){
        double pivot = pnl[(l+r)>>1];
        int i=l, j=r;
        while(i<=j){
            while(pnl[i]<pivot) i++;
            while(pnl[j]>pivot) j--;
            if(i<=j){ double t=pnl[i]; pnl[i]=pnl[j]; pnl[j]=t; i++; j--; }
        }
        if(idx<=j) r=j; else if(idx>=i) l=i; else break;
    }
    volatile double var = -pnl[idx];
    long long t1 = now_ns();
    (void)var;
    printf("TASK=var_mc_qsel,N=%d,TIME_NS=%lld\n", N, (t1 - t0));
    free(pnl);
    return 0;
}
