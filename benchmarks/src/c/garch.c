// FILE: benchmarks/src/c/garch.c
// Purpose: GARCH volatility modeling benchmark
#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {
    int n_obs = (argc > 1) ? atoi(argv[1]) : 10000;
    
    // GARCH(1,1) parameters
    double omega = 0.0001;  // Constant term
    double alpha = 0.1;     // ARCH coefficient
    double beta = 0.85;     // GARCH coefficient
    
    long long t0 = now_ns();
    
    // Allocate arrays
    double *returns = malloc(n_obs * sizeof(double));
    double *variances = malloc(n_obs * sizeof(double));
    
    // Generate random returns and calculate GARCH variances
    for (int i = 0; i < n_obs; i++) {
        returns[i] = 0.01 * (i % 100 - 50) / 50.0; // Simulated returns
        
        double var_prev = (i == 0) ? omega / (1 - alpha - beta) : variances[i - 1];
        variances[i] = omega + alpha * returns[i] * returns[i] + beta * var_prev;
    }
    
    // Calculate volatility
    double vol_sum = 0.0;
    for (int i = 0; i < n_obs; i++) {
        vol_sum += sqrt(variances[i]);
    }
    
    long long t1 = now_ns();
    
    printf("TASK=garch,N=%d,TIME_NS=%lld,VOL_SUM=%.6f\n", 
           n_obs, (t1 - t0), vol_sum);
    
    free(returns);
    free(variances);
    return 0;
}

