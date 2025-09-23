#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
static inline long long now_ns(){struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return (long long)ts.tv_sec*1000000000LL+ts.tv_nsec;}
int main(int argc,char**argv){
    int n_points = (argc>1)?atoi(argv[1]):1024;
    long long t0=now_ns();
    double*signal=malloc(n_points*sizeof(double));
    double*fft_real=malloc(n_points*sizeof(double));
    double*fft_imag=malloc(n_points*sizeof(double));
    for(int i=0;i<n_points;i++){
        double t=i*2.0*M_PI/n_points;
        signal[i]=sin(t)+0.5*sin(3*t)+0.25*sin(5*t);
    }
    for(int k=0;k<n_points;k++){
        double real_sum=0.0, imag_sum=0.0;
        for(int n=0;n<n_points;n++){
            double angle=-2.0*M_PI*k*n/n_points;
            real_sum += signal[n]*cos(angle);
            imag_sum += signal[n]*sin(angle);
        }
        fft_real[k]=real_sum; fft_imag[k]=imag_sum;
    }
    double power_sum=0.0;
    for(int i=0;i<n_points;i++){
        double power=fft_real[i]*fft_real[i]+fft_imag[i]*fft_imag[i];
        power_sum += power;
    }
    long long t1=now_ns();
    printf("TASK=fft,N=%d,TIME_NS=%lld,POWER_SUM=%.6f\n", n_points, (t1-t0), power_sum);
    free(signal); free(fft_real); free(fft_imag); return 0;
}
