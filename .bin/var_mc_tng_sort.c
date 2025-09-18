#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
static inline long long now_ns(){struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return (long long)ts.tv_sec*1000000000LL+ts.tv_nsec;}
static uint64_t xs=0x9E3779B97F4A7C15ULL;
static inline void s(uint64_t v){ xs = v? v:0x9E3779B97F4A7C15ULL; }
static inline uint64_t n64(){ uint64_t x=xs; x^=x>>12; x^=x<<25; x^=x>>27; xs=x; return x*0x2545F4914F6CDD1DULL; }
static inline double u01(){ return (n64()>>11) * (1.0/9007199254740992.0); }
static inline double z01(){ double u1=u01(); if(u1<1e-300) u1=1e-300; double u2=u01(); return sqrt(-2.0*log(u1))*cos(2.0*M_PI*u2); }
static int cmpd(const void*a,const void*b){double da=*(const double*)a, db=*(const double*)b; return (da>db)-(da<db);}
int main(int argc,char**argv){
    int N = (argc>1)?atoi(argv[1]):1000000;
    int steps = (argc>2)?atoi(argv[2]):1;
    double alpha = (argc>3)?atof(argv[3]):0.99;
    const double S0=100.0, mu=0.05, sigma=0.20;
    double T=(double)steps/252.0, dt=T/(double)steps;
    double*loss=(double*)malloc(sizeof(double)*N);
    s(123456789u);
    long long t0=now_ns();
    for(int i=0;i<N;i++){
        double S=S0;
        for(int k=0;k<steps;k++){
            double z=z01();
            double drift=(mu-0.5*sigma*sigma)*dt;
            double diff=sigma*sqrt(dt)*z;
            S*=exp(drift+diff);
        }
        loss[i]=-(S-S0);
    }
    qsort(loss,N,sizeof(double),cmpd);
    int idx = N-1 - (int)((1.0 - alpha)*N);
    if(idx<0) idx=0; if(idx>=N) idx=N-1;
    double var=loss[idx];
    long long t1=now_ns();
    printf("TASK=var_mc_sort,N=%d,TIME_NS=%lld,VAR=%.6f\n",N,(t1-t0),var);
    free(loss);
    return 0;
}
