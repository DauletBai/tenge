#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
static inline long long now_ns(){struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return (long long)ts.tv_sec*1000000000LL+ts.tv_nsec;}
int main(int argc,char**argv){
    int n_obs = (argc>1)?atoi(argv[1]):10000;
    double omega=0.0001, alpha=0.1, beta=0.85;
    long long t0=now_ns();
    double*returns=malloc(n_obs*sizeof(double));
    double*variances=malloc(n_obs*sizeof(double));
    for(int i=0;i<n_obs;i++){
        returns[i] = 0.01*(i%100-50)/50.0;
        double var_prev = (i==0)?omega/(1-alpha-beta):variances[i-1];
        variances[i] = omega + alpha*returns[i]*returns[i] + beta*var_prev;
    }
    double vol_sum=0.0;
    for(int i=0;i<n_obs;i++) vol_sum += sqrt(variances[i]);
    long long t1=now_ns();
    printf("TASK=garch,N=%d,TIME_NS=%lld,VOL_SUM=%.6f\n", n_obs, (t1-t0), vol_sum);
    free(returns); free(variances); return 0;
}
