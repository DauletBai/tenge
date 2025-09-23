// benchmarks/src/c/nbody.c
// Naive O(N^2) N-body stepper (Structure-of-Arrays). Uses shared runtime now_ns().

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
    // Deterministic initialization for reproducibility
    // Puts bodies on three sinusoids with small velocities.
    for (size_t i = 0; i < n; ++i) {
        double t = (double)i;
        x[i]  = sin(0.001 * t) * 100.0;
        y[i]  = cos(0.0013 * t) * 100.0;
        z[i]  = sin(0.00077 * t) * 100.0;
        vx[i] = 0.001 * cos(0.0002 * t);
        vy[i] = 0.001 * sin(0.0003 * t);
        vz[i] = 0.001 * cos(0.0005 * t);
        m[i]  = 1.0; // unit mass
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

    if (!x || !y || !z || !vx || !vy || !vz || !m) {
        fprintf(stderr, "Allocation failed\n");
        return 1;
    }

    init_state(N, x, y, z, vx, vy, vz, m);

    const double eps2 = SOFTENING * SOFTENING;

    long long t0 = now_ns();

    for (int s = 0; s < steps; ++s) {
        // Compute accelerations and update velocities
        for (size_t i = 0; i < N; ++i) {
            double ax = 0.0, ay = 0.0, az = 0.0;
            const double xi = x[i], yi = y[i], zi = z[i];

            for (size_t j = 0; j < N; ++j) {
                if (j == i) continue;
                double dx = x[j] - xi;
                double dy = y[j] - yi;
                double dz = z[j] - zi;
                double r2 = dx*dx + dy*dy + dz*dz + eps2;
                double inv_r = rsqrt(r2);
                double inv_r3 = inv_r * inv_r * inv_r;
                double s_ = m[j] * inv_r3;
                ax += dx * s_;
                ay += dy * s_;
                az += dz * s_;
            }

            vx[i] += ax * dt;
            vy[i] += ay * dt;
            vz[i] += az * dt;
        }

        // Integrate positions
        for (size_t i = 0; i < N; ++i) {
            x[i] += vx[i] * dt;
            y[i] += vy[i] * dt;
            z[i] += vz[i] * dt;
        }
    }

    long long t1 = now_ns();

    // Emit a uniform, machine-parsable line (same style as other C benches)
    printf("TASK=nbody,N=%zu,STEPS=%d,DT=%.6f,TIME_NS=%lld\n", N, steps, dt, (t1 - t0));

    free(x); free(y); free(z);
    free(vx); free(vy); free(vz);
    free(m);
    return 0;
}