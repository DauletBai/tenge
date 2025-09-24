
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "runtime.h"    // now_ns()

static inline uint64_t xorshift64(uint64_t* s){
    uint64_t x=*s; x^=x>>12; x^=x<<25; x^=x>>27; *s=x; return x*0x2545F4914F6CDD1DULL;
}
static inline double u01(uint64_t* s){ return (xorshift64(s)>>11)*(1.0/9007199254740992.0); }

static int cmp_d(const void* a,const void* b){
    double x=*(const double*)a, y=*(const double*)b;
    return (x>y)-(x<y);
}
int main(int argc, char** argv){
    int N    = (argc>1)? atoi(argv[1]) : 1000000;
    int steps= (argc>2)? atoi(argv[2]) : 1;
    double a = (argc>3)? atof(argv[3]) : 0.99;
    double* pnl = (double*)malloc(N*sizeof(double));
    if(!pnl){ fprintf(stderr,"oom\n"); return 1; }
    long long t0 = now_ns();
    uint64_t s=123456789;
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
    qsort(pnl, N, sizeof(double), cmp_d);
    int idx = (int)((1.0-a)*N); if(idx<0) idx=0; if(idx>=N) idx=N-1;
    volatile double var = -pnl[idx];
    long long t1 = now_ns();
    (void)var;
    printf("TASK=var_mc_sort,N=%d,TIME_NS=%lld\n", N, (t1 - t0));
    free(pnl);
    return 0;
}
