#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
static inline long long now_ns(){struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return (long long)ts.tv_sec*1000000000LL+ts.tv_nsec;}

// Agglutinative Loop Fusion: Combine returns calculation, covariance matrix, and portfolio variance
// This eliminates intermediate arrays and improves cache efficiency
static inline double agglutinative_portfolio_optimization(int n_assets) {
    // Allocate aligned memory for better SIMD performance
    double* returns = aligned_alloc(32, n_assets * sizeof(double));
    double* weights = aligned_alloc(32, n_assets * sizeof(double));
    double* temp_row = aligned_alloc(32, n_assets * sizeof(double));
    
    if (!returns || !weights || !temp_row) return 0.0;
    
    // Agglutinative fusion: Calculate returns, weights, and covariance in one pass
    double portfolio_var = 0.0;
    const double weight = 1.0 / n_assets;
    
    for (int i = 0; i < n_assets; i++) {
        // Step 1: Calculate return (fused with weight assignment)
        returns[i] = 0.01 + 0.02 * (i % 10) / 10.0;
        weights[i] = weight;
        
        // Step 2: Calculate covariance matrix row and accumulate variance (fused)
        double row_contribution = 0.0;
        for (int j = 0; j < n_assets; j++) {
            double cov_ij = (i == j) ? 0.04 : 0.01 * (i + j) / (2.0 * n_assets);
            temp_row[j] = cov_ij;
            row_contribution += weights[i] * weights[j] * cov_ij;
        }
        portfolio_var += row_contribution;
    }
    
    free(returns); free(weights); free(temp_row);
    return portfolio_var;
}

int main(int argc,char**argv){
    int n_assets = (argc>1)?atoi(argv[1]):100;
    long long t0=now_ns();
    
    // Agglutinative optimization: Single fused operation
    double portfolio_var = agglutinative_portfolio_optimization(n_assets);
    
    long long t1=now_ns();
    printf("TASK=portfolio_opt,N=%d,TIME_NS=%lld,PORTFOLIO_VAR=%.6f\n", n_assets, (t1-t0), portfolio_var);
    return 0;
}
