// FILE: benchmarks/src/c/portfolio_opt.c
// Purpose: Portfolio optimization benchmark
#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {
    int n_assets = (argc > 1) ? atoi(argv[1]) : 100;
    
    long long t0 = now_ns();
    
    // Allocate arrays
    double *returns = malloc(n_assets * sizeof(double));
    double **cov_matrix = malloc(n_assets * sizeof(double*));
    double *weights = malloc(n_assets * sizeof(double));
    
    for (int i = 0; i < n_assets; i++) {
        cov_matrix[i] = malloc(n_assets * sizeof(double));
    }
    
    // Generate random returns
    for (int i = 0; i < n_assets; i++) {
        returns[i] = 0.01 + 0.02 * (i % 10) / 10.0; // Simulated returns
    }
    
    // Generate covariance matrix
    for (int i = 0; i < n_assets; i++) {
        for (int j = 0; j < n_assets; j++) {
            if (i == j) {
                cov_matrix[i][j] = 0.04; // Variance
            } else {
                cov_matrix[i][j] = 0.01 * (i + j) / (2.0 * n_assets); // Covariance
            }
        }
    }
    
    // Simple portfolio optimization (equal weights)
    for (int i = 0; i < n_assets; i++) {
        weights[i] = 1.0 / n_assets;
    }
    
    // Calculate portfolio variance
    double portfolio_var = 0.0;
    for (int i = 0; i < n_assets; i++) {
        for (int j = 0; j < n_assets; j++) {
            portfolio_var += weights[i] * weights[j] * cov_matrix[i][j];
        }
    }
    
    long long t1 = now_ns();
    
    printf("TASK=portfolio_opt,N=%d,TIME_NS=%lld,PORTFOLIO_VAR=%.6f\n", 
           n_assets, (t1 - t0), portfolio_var);
    
    // Cleanup
    for (int i = 0; i < n_assets; i++) {
        free(cov_matrix[i]);
    }
    free(cov_matrix);
    free(returns);
    free(weights);
    
    return 0;
}

