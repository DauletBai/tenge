// FILE: benchmarks/src/c/yield_curve.c
// Purpose: Yield curve modeling benchmark for central bank applications
#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {
    int n_points = (argc > 1) ? atoi(argv[1]) : 1000;
    
    // Nelson-Siegel parameters
    double beta0 = 0.05;  // Long-term rate
    double beta1 = -0.02; // Short-term component
    double beta2 = 0.01;  // Medium-term component
    double tau = 2.0;     // Decay parameter
    
    long long t0 = now_ns();
    
    double sum = 0.0;
    for (int i = 0; i < n_points; i++) {
        double t = (i + 1) * 0.1; // Time to maturity
        double yield_val = beta0 + beta1 * (1 - exp(-t/tau)) / (t/tau) + 
                           beta2 * ((1 - exp(-t/tau)) / (t/tau) - exp(-t/tau));
        sum += yield_val;
    }
    
    long long t1 = now_ns();
    
    printf("TASK=yield_curve,N=%d,TIME_NS=%lld,SUM=%.6f\n", 
           n_points, (t1 - t0), sum);
    return 0;
}

