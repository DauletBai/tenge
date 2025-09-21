// FILE: benchmarks/src/c/matrix_ops.c
// Purpose: Matrix operations benchmark
#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {
    int matrix_size = (argc > 1) ? atoi(argv[1]) : 200;
    
    long long t0 = now_ns();
    
    // Allocate matrices
    double **matrix_a = malloc(matrix_size * sizeof(double*));
    double **matrix_b = malloc(matrix_size * sizeof(double*));
    double **matrix_c = malloc(matrix_size * sizeof(double*));
    
    for (int i = 0; i < matrix_size; i++) {
        matrix_a[i] = malloc(matrix_size * sizeof(double));
        matrix_b[i] = malloc(matrix_size * sizeof(double));
        matrix_c[i] = malloc(matrix_size * sizeof(double));
    }
    
    // Generate matrix A
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            matrix_a[i][j] = (i + j) * 0.01;
        }
    }
    
    // Generate matrix B
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            matrix_b[i][j] = (i - j) * 0.01;
        }
    }
    
    // Matrix multiplication C = A * B
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            double sum = 0.0;
            for (int k = 0; k < matrix_size; k++) {
                sum += matrix_a[i][k] * matrix_b[k][j];
            }
            matrix_c[i][j] = sum;
        }
    }
    
    // Calculate trace
    double trace = 0.0;
    for (int i = 0; i < matrix_size; i++) {
        trace += matrix_c[i][i];
    }
    
    long long t1 = now_ns();
    
    printf("TASK=matrix_ops,N=%d,TIME_NS=%lld,TRACE=%.6f\n", 
           matrix_size, (t1 - t0), trace);
    
    // Cleanup
    for (int i = 0; i < matrix_size; i++) {
        free(matrix_a[i]);
        free(matrix_b[i]);
        free(matrix_c[i]);
    }
    free(matrix_a);
    free(matrix_b);
    free(matrix_c);
    
    return 0;
}

