#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
static inline long long now_ns(){struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return (long long)ts.tv_sec*1000000000LL+ts.tv_nsec;}
int main(int argc,char**argv){
    int n_points = (argc>1)?atoi(argv[1]):1000;
    double beta0=0.05, beta1=-0.02, beta2=0.01, tau=2.0;
    long long t0=now_ns();
    double sum=0.0;
    for(int i=0;i<n_points;i++){
        double t=(i+1)*0.1;
        double yield_val = beta0 + beta1*(1-exp(-t/tau))/(t/tau) + beta2*((1-exp(-t/tau))/(t/tau) - exp(-t/tau));
        sum += yield_val;
    }
    long long t1=now_ns();
    printf("TASK=yield_curve,N=%d,TIME_NS=%lld,SUM=%.6f\n", n_points, (t1-t0), sum);
    return 0;
}
