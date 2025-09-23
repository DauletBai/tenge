// benchmarks/src/c/nbody_sym.c
// Symmetric O(N^2) kernel (Newton's 3rd law). SoA layout, shared runtime now_ns().

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "runtime.h"

#ifndef SOFTENING
#define SOFTENING 1e-9
#endif

static inline double rsqrt(double x) {
    return 1.0 / sqrt(x);
}

static void init_state(size_t n, double *x, double *y, double *z,
                       double *vx, double *vy, double *vz, double *m)
{
    for (size_t i = 0; i < n; ++i) {
        double t = (double)i;
        x[i]  = sin(0.001 * t) * 100.0;
        y[i]  = cos(0.0013 * t) * 100.0;
        z[i]  = sin(0.00077 * t) * 100.0;
        vx[i] = 0.001 * cos(0.0002 * t);
        vy[i] = 0.001 * sin(0.0003 * t);
        vz[i] = 0.001 * cos(0.0005 * t);
        m[i]  = 1.0;
    }
}

int main(int argc, char **argv)
{
    const size_t N      = (argc > 1) ? (size_t)atoll(argv[1]) : 4096;
    const int    steps  = (argc > 2) ? atoi(argv[2]) : 10;
    const double dt     = (argc > 3) ? atof(argv[3]) : 0.001;

    double *x  = (double*)malloc(N * sizeof(double));
    double *y  = (double*)malloc(N * sizeof(double));
    double *z  = (double*)malloc(N * sizeof(double));
    double *vx = (double*)malloc(N * sizeof(double));
    double *vy = (double*)malloc(N * sizeof(double));
    double *vz = (double*)malloc(N * sizeof(double));
    double *m  = (double*)malloc(N * sizeof(double));
    double *ax = (double*)malloc(N * sizeof(double));
    double *ay = (double*)malloc(N * sizeof(double));
    double *az = (double*)malloc(N * sizeof(double));

    if (!x || !y || !z || !vx || !vy || !vz || !m || !ax || !ay || !az) {
        fprintf(stderr, "Allocation failed\n");
        return 1;
    }

    init_state(N, x, y, z, vx, vy, vz, m);

    const double eps2 = SOFTENING * SOFTENING;

    long long t0 = now_ns();

    for (int s = 0; s < steps; ++s) {
        // Zero accelerations
        for (size_t i = 0; i < N; ++i) { ax[i] = ay[i] = az[i] = 0.0; }

        // Symmetric force accumulation: j > i
        for (size_t i = 0; i < N; ++i) {
            const double xi = x[i], yi = y[i], zi = z[i];
            for (size_t j = i + 1; j < N; ++j) {
                double dx = x[j] - xi;
                double dy = y[j] - yi;
                double dz = z[j] - zi;
                double r2 = dx*dx + dy*dy + dz*dz + eps2;
                double inv_r = rsqrt(r2);
                double inv_r3 = inv_r * inv_r * inv_r;

                double s_i =  m[j] * inv_r3;
                double s_j = -m[i] * inv_r3; // equal and opposite

                ax[i] += dx * s_i; ay[i] += dy * s_i; az[i] += dz * s_i;
                ax[j] += dx * s_j; ay[j] += dy * s_j; az[j] += dz * s_j;
            }
        }

        // Velocity and position updates
        for (size_t i = 0; i < N; ++i) {
            vx[i] += ax[i] * dt;
            vy[i] += ay[i] * dt;
            vz[i] += az[i] * dt;

            x[i]  += vx[i] * dt;
            y[i]  += vy[i] * dt;
            z[i]  += vz[i] * dt;
        }
    }

    long long t1 = now_ns();

    printf("TASK=nbody_sym,N=%zu,STEPS=%d,DT=%.6f,TIME_NS=%lld\n", N, steps, dt, (t1 - t0));

    free(x); free(y); free(z);
    free(vx); free(vy); free(vz);
    free(m);  free(ax); free(ay); free(az);
    return 0;
}