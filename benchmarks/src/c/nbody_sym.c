// FILE: benchmarks/src/c/nbody_sym.c
// Purpose: Symmetric N-body (i<j) with SoA layout, tiling and temporary per-tile buffers.
// Integrator: velocity-Verlet. Masses = 1.0. Always prints TIME_NS (even on allocation failure).

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <alloca.h>
#include <stdint.h>

static inline long long now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

// Use posix_memalign for macOS portability and guaranteed alignment.
static void* alloc_aligned(size_t align, size_t bytes) {
  void* p = NULL;
  if (posix_memalign(&p, align, bytes) != 0) return NULL;
  return p;
}

// Tiny PRNG for initialization (xorshift64*).
static uint64_t xs = 0x9E3779B97F4A7C15ULL;
static inline uint64_t n64(void) {
  uint64_t x = xs;
  x ^= x >> 12; x ^= x << 25; x ^= x >> 27;
  xs = x;
  return x * 0x2545F4914F6CDD1DULL;
}
static inline double u01(void) {
  return (double)(n64() >> 11) * (1.0 / 9007199254740992.0);
}

int main(int argc, char** argv) {
  const int N     = (argc > 1) ? atoi(argv[1]) : 4096;
  const int STEPS = (argc > 2) ? atoi(argv[2]) : 10;
  const double dt = (argc > 3) ? atof(argv[3]) : 1e-3;

  double *px = (double*)alloc_aligned(64, sizeof(double) * N);
  double *py = (double*)alloc_aligned(64, sizeof(double) * N);
  double *pz = (double*)alloc_aligned(64, sizeof(double) * N);
  double *vx = (double*)alloc_aligned(64, sizeof(double) * N);
  double *vy = (double*)alloc_aligned(64, sizeof(double) * N);
  double *vz = (double*)alloc_aligned(64, sizeof(double) * N);
  double *ax = (double*)alloc_aligned(64, sizeof(double) * N);
  double *ay = (double*)alloc_aligned(64, sizeof(double) * N);
  double *az = (double*)alloc_aligned(64, sizeof(double) * N);

  if (!px || !py || !pz || !vx || !vy || !vz || !ax || !ay || !az) {
    fprintf(stderr, "nbody_sym: allocation failed\n");
    printf("TASK=nbody_sym,N=%d,TIME_NS=0,ENERGY=0.0\n", N);
    return 1;
  }

  xs = 123456789ULL;
  for (int i = 0; i < N; i++) {
    px[i] = u01(); py[i] = u01(); pz[i] = u01();
    vx[i] = (u01() - 0.5) * 1e-3;
    vy[i] = (u01() - 0.5) * 1e-3;
    vz[i] = (u01() - 0.5) * 1e-3;
    ax[i] = ay[i] = az[i] = 0.0;
  }

  const double G = 1.0;
  const double eps2 = 1e-9;
  const int TILE = 64;

  // Initial accelerations using symmetric accumulation and tiling.
  for (int i = 0; i < N; i++) { ax[i] = ay[i] = az[i] = 0.0; }

  for (int i0 = 0; i0 < N; i0 += TILE) {
    const int i1 = (i0 + TILE < N) ? (i0 + TILE) : N;

    // Intra-tile symmetric interactions.
    for (int i = i0; i < i1; i++) {
      for (int j = i + 1; j < i1; j++) {
        const double rx = px[j] - px[i];
        const double ry = py[j] - py[i];
        const double rz = pz[j] - pz[i];
        const double r2 = rx*rx + ry*ry + rz*rz + eps2;
        const double inv = 1.0 / sqrt(r2 * r2 * r2);
        const double s = G * inv;
        const double fx = rx * s, fy = ry * s, fz = rz * s;
        ax[i] += fx; ay[i] += fy; az[i] += fz;
        ax[j] -= fx; ay[j] -= fy; az[j] -= fz;
      }
    }

    // Inter-tile symmetric interactions with temporary buffers.
    for (int j0 = i1; j0 < N; j0 += TILE) {
      const int j1 = (j0 + TILE < N) ? (j0 + TILE) : N;
      const int Ti = i1 - i0, Tj = j1 - j0;

      double *taxi = (double*)alloca(sizeof(double) * Ti);
      double *tayi = (double*)alloca(sizeof(double) * Ti);
      double *tazi = (double*)alloca(sizeof(double) * Ti);
      double *taxj = (double*)alloca(sizeof(double) * Tj);
      double *tayj = (double*)alloca(sizeof(double) * Tj);
      double *tazj = (double*)alloca(sizeof(double) * Tj);

      for (int t = 0; t < Ti; t++) { taxi[t] = tayi[t] = tazi[t] = 0.0; }
      for (int t = 0; t < Tj; t++) { taxj[t] = tayj[t] = tazj[t] = 0.0; }

      for (int ii = 0; ii < Ti; ii++) {
        const int i = i0 + ii;
        const double pix = px[i], piy = py[i], piz = pz[i];
        for (int jj = 0; jj < Tj; jj++) {
          const int j = j0 + jj;
          const double rx = px[j] - pix;
          const double ry = py[j] - piy;
          const double rz = pz[j] - piz;
          const double r2 = rx*rx + ry*ry + rz*rz + eps2;
          const double inv = 1.0 / sqrt(r2 * r2 * r2);
          const double s = G * inv;
          const double fx = rx * s, fy = ry * s, fz = rz * s;
          taxi[ii] += fx; tayi[ii] += fy; tazi[ii] += fz;
          taxj[jj] -= fx; tayj[jj] -= fy; tazj[jj] -= fz;
        }
      }

      for (int ii = 0; ii < Ti; ii++) { ax[i0 + ii] += taxi[ii]; ay[i0 + ii] += tayi[ii]; az[i0 + ii] += tazi[ii]; }
      for (int jj = 0; jj < Tj; jj++) { ax[j0 + jj] += taxj[jj]; ay[j0 + jj] += tayj[jj]; az[j0 + jj] += tazj[jj]; }
    }
  }

  long long t0 = now_ns();

  for (int s = 0; s < STEPS; s++) {
    // Drift
    for (int i = 0; i < N; i++) {
      px[i] += vx[i] * dt + 0.5 * ax[i] * dt * dt;
      py[i] += vy[i] * dt + 0.5 * ay[i] * dt * dt;
      pz[i] += vz[i] * dt + 0.5 * az[i] * dt * dt;
    }
    for (int i = 0; i < N; i++) { ax[i] = ay[i] = az[i] = 0.0; }

    // Recompute accelerations (symmetric + tiling).
    for (int i0 = 0; i0 < N; i0 += TILE) {
      const int i1 = (i0 + TILE < N) ? (i0 + TILE) : N;

      for (int i = i0; i < i1; i++) {
        for (int j = i + 1; j < i1; j++) {
          const double rx = px[j] - px[i];
          const double ry = py[j] - py[i];
          const double rz = pz[j] - pz[i];
          const double r2 = rx*rx + ry*ry + rz*rz + eps2;
          const double inv = 1.0 / sqrt(r2 * r2 * r2);
          const double s = G * inv;
          const double fx = rx * s, fy = ry * s, fz = rz * s;
          ax[i] += fx; ay[i] += fy; az[i] += fz;
          ax[j] -= fx; ay[j] -= fy; az[j] -= fz;
        }
      }

      for (int j0 = i1; j0 < N; j0 += TILE) {
        const int j1 = (j0 + TILE < N) ? (j0 + TILE) : N;
        const int Ti = i1 - i0, Tj = j1 - j0;

        double *taxi = (double*)alloca(sizeof(double) * Ti);
        double *tayi = (double*)alloca(sizeof(double) * Ti);
        double *tazi = (double*)alloca(sizeof(double) * Ti);
        double *taxj = (double*)alloca(sizeof(double) * Tj);
        double *tayj = (double*)alloca(sizeof(double) * Tj);
        double *tazj = (double*)alloca(sizeof(double) * Tj);

        for (int t = 0; t < Ti; t++) { taxi[t] = tayi[t] = tazi[t] = 0.0; }
        for (int t = 0; t < Tj; t++) { taxj[t] = tayj[t] = tazj[t] = 0.0; }

        for (int ii = 0; ii < Ti; ii++) {
          const int i = i0 + ii;
          const double pix = px[i], piy = py[i], piz = pz[i];
          for (int jj = 0; jj < Tj; jj++) {
            const int j = j0 + jj;
            const double rx = px[j] - pix;
            const double ry = py[j] - piy;
            const double rz = pz[j] - piz;
            const double r2 = rx*rx + ry*ry + rz*rz + eps2;
            const double inv = 1.0 / sqrt(r2 * r2 * r2);
            const double s = G * inv;
            const double fx = rx * s, fy = ry * s, fz = rz * s;
            taxi[ii] += fx; tayi[ii] += fy; tazi[ii] += fz;
            taxj[jj] -= fx; tayj[jj] -= fy; tazj[jj] -= fz;
          }
        }

        for (int ii = 0; ii < Ti; ii++) { ax[i0 + ii] += taxi[ii]; ay[i0 + ii] += tayi[ii]; az[i0 + ii] += tazi[ii]; }
        for (int jj = 0; jj < Tj; jj++) { ax[j0 + jj] += taxj[jj]; ay[j0 + jj] += tayj[jj]; az[j0 + jj] += tazj[jj]; }
      }
    }

    // Kick
    for (int i = 0; i < N; i++) {
      vx[i] += ax[i] * dt;
      vy[i] += ay[i] * dt;
      vz[i] += az[i] * dt;
    }
  }

  long long t1 = now_ns();

  // Optional energy compute for sanity.
  double KE = 0.0, PE = 0.0;
  for (int i = 0; i < N; i++) KE += 0.5 * (vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i]);
  for (int i = 0; i < N; i++) {
    for (int j = i + 1; j < N; j++) {
      const double rx = px[j] - px[i];
      const double ry = py[j] - py[i];
      const double rz = pz[j] - pz[i];
      const double r = sqrt(rx*rx + ry*ry + rz*rz + 1e-9);
      PE += -1.0 / r;
    }
  }
  const double E = KE + PE;

  printf("TASK=nbody_sym,N=%d,TIME_NS=%lld,ENERGY=%.9f\n", N, (t1 - t0), E);

  free(px); free(py); free(pz); free(vx); free(vy); free(vz); free(ax); free(ay); free(az);
  return 0;
}