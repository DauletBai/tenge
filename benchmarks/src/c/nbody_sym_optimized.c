#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <alloca.h>
#include <stdint.h>
#include <immintrin.h>

static inline long long now_ns(void){
  struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec*1000000000LL + ts.tv_nsec;
}
static void* alloc_aligned(size_t align, size_t bytes){
  void* p=NULL; if(posix_memalign(&p, align, bytes)!=0) return NULL; return p;
}
static uint64_t xs=0x9E3779B97F4A7C15ULL;
static inline uint64_t n64(void){ uint64_t x=xs; x^=x>>12; x^=x<<25; x^=x>>27; xs=x; return x*0x2545F4914F6CDD1DULL; }
static inline double u01(void){ return (double)(n64()>>11) * (1.0/9007199254740992.0); }

// SIMD Archetype Functions for Vectorized Operations
static inline void simd_update_positions(double* px, double* py, double* pz, 
                                        double* vx, double* vy, double* vz,
                                        double* ax, double* ay, double* az,
                                        double dt, int N) {
    const __m256d dt_vec = _mm256_set1_pd(dt);
    const __m256d dt2_vec = _mm256_set1_pd(0.5 * dt * dt);
    
    for (int i = 0; i < N; i += 4) {
        // Load 4 positions, velocities, and accelerations
        __m256d px_vec = _mm256_load_pd(&px[i]);
        __m256d py_vec = _mm256_load_pd(&py[i]);
        __m256d pz_vec = _mm256_load_pd(&pz[i]);
        __m256d vx_vec = _mm256_load_pd(&vx[i]);
        __m256d vy_vec = _mm256_load_pd(&vy[i]);
        __m256d vz_vec = _mm256_load_pd(&vz[i]);
        __m256d ax_vec = _mm256_load_pd(&ax[i]);
        __m256d ay_vec = _mm256_load_pd(&ay[i]);
        __m256d az_vec = _mm256_load_pd(&az[i]);
        
        // Update positions: p += v*dt + 0.5*a*dt*dt
        px_vec = _mm256_add_pd(px_vec, _mm256_add_pd(_mm256_mul_pd(vx_vec, dt_vec), _mm256_mul_pd(ax_vec, dt2_vec)));
        py_vec = _mm256_add_pd(py_vec, _mm256_add_pd(_mm256_mul_pd(vy_vec, dt_vec), _mm256_mul_pd(ay_vec, dt2_vec)));
        pz_vec = _mm256_add_pd(pz_vec, _mm256_add_pd(_mm256_mul_pd(vz_vec, dt_vec), _mm256_mul_pd(az_vec, dt2_vec)));
        
        // Store updated positions
        _mm256_store_pd(&px[i], px_vec);
        _mm256_store_pd(&py[i], py_vec);
        _mm256_store_pd(&pz[i], pz_vec);
    }
}

static inline void simd_update_velocities(double* vx, double* vy, double* vz,
                                        double* ax, double* ay, double* az,
                                        double dt, int N) {
    const __m256d dt_vec = _mm256_set1_pd(dt);
    
    for (int i = 0; i < N; i += 4) {
        // Load 4 velocities and accelerations
        __m256d vx_vec = _mm256_load_pd(&vx[i]);
        __m256d vy_vec = _mm256_load_pd(&vy[i]);
        __m256d vz_vec = _mm256_load_pd(&vz[i]);
        __m256d ax_vec = _mm256_load_pd(&ax[i]);
        __m256d ay_vec = _mm256_load_pd(&ay[i]);
        __m256d az_vec = _mm256_load_pd(&az[i]);
        
        // Update velocities: v += a*dt
        vx_vec = _mm256_add_pd(vx_vec, _mm256_mul_pd(ax_vec, dt_vec));
        vy_vec = _mm256_add_pd(vy_vec, _mm256_mul_pd(ay_vec, dt_vec));
        vz_vec = _mm256_add_pd(vz_vec, _mm256_mul_pd(az_vec, dt_vec));
        
        // Store updated velocities
        _mm256_store_pd(&vx[i], vx_vec);
        _mm256_store_pd(&vy[i], vy_vec);
        _mm256_store_pd(&vz[i], vz_vec);
    }
}

int main(int argc,char**argv){
  const int N     = (argc>1)?atoi(argv[1]):4096;
  const int STEPS = (argc>2)?atoi(argv[2]):10;
  const double dt = (argc>3)?atof(argv[3]):1e-3;

  double *px=(double*)alloc_aligned(64,sizeof(double)*N);
  double *py=(double*)alloc_aligned(64,sizeof(double)*N);
  double *pz=(double*)alloc_aligned(64,sizeof(double)*N);
  double *vx=(double*)alloc_aligned(64,sizeof(double)*N);
  double *vy=(double*)alloc_aligned(64,sizeof(double)*N);
  double *vz=(double*)alloc_aligned(64,sizeof(double)*N);
  double *ax=(double*)alloc_aligned(64,sizeof(double)*N);
  double *ay=(double*)alloc_aligned(64,sizeof(double)*N);
  double *az=(double*)alloc_aligned(64,sizeof(double)*N);

  if(!px||!py||!pz||!vx||!vy||!vz||!ax||!ay||!az){
    fprintf(stderr,"nbody_sym(AOT): allocation failed\n");
    printf("TASK=nbody_sym,N=%d,TIME_NS=0,ENERGY=0.0\n", N);
    return 1;
  }

  xs=123456789ULL;
  for(int i=0;i<N;i++){
    px[i]=u01(); py[i]=u01(); pz[i]=u01();
    vx[i]=(u01()-0.5)*1e-3; vy[i]=(u01()-0.5)*1e-3; vz[i]=(u01()-0.5)*1e-3;
    ax[i]=ay[i]=az[i]=0.0;
  }

  const double G=1.0, eps2=1e-9; const int TILE=64;

  for(int i=0;i<N;i++){ ax[i]=ay[i]=az[i]=0.0; }
  for(int i0=0;i0<N;i0+=TILE){
    const int i1=(i0+TILE<N)?(i0+TILE):N;
    for(int i=i0;i<i1;i++){
      for(int j=i+1;j<i1;j++){
        const double rx=px[j]-px[i], ry=py[j]-py[i], rz=pz[j]-pz[i];
        const double r2=rx*rx+ry*ry+rz*rz+eps2;
        const double inv=1.0/sqrt(r2*r2*r2);
        const double s=G*inv;
        const double fx=rx*s, fy=ry*s, fz=rz*s;
        ax[i]+=fx; ay[i]+=fy; az[i]+=fz;
        ax[j]-=fx; ay[j]-=fy; az[j]-=fz;
      }
    }
    for(int j0=i1;j0<N;j0+=TILE){
      const int j1=(j0+TILE<N)?(j0+TILE):N;
      const int Ti=i1-i0, Tj=j1-j0;
      double *taxi=(double*)alloca(sizeof(double)*Ti);
      double *tayi=(double*)alloca(sizeof(double)*Ti);
      double *tazi=(double*)alloca(sizeof(double)*Ti);
      double *taxj=(double*)alloca(sizeof(double)*Tj);
      double *tayj=(double*)alloca(sizeof(double)*Tj);
      double *tazj=(double*)alloca(sizeof(double)*Tj);
      for(int t=0;t<Ti;t++){ taxi[t]=tayi[t]=tazi[t]=0.0; }
      for(int t=0;t<Tj;t++){ taxj[t]=tayj[t]=tazj[t]=0.0; }

      for(int ii=0; ii<Ti; ii++){
        const int i=i0+ii; const double pix=px[i], piy=py[i], piz=pz[i];
        for(int jj=0; jj<Tj; jj++){
          const int j=j0+jj;
          const double rx=px[j]-pix, ry=py[j]-piy, rz=pz[j]-piz;
          const double r2=rx*rx+ry*ry+rz*rz + eps2;
          const double inv=1.0/sqrt(r2*r2*r2);
          const double s=G*inv;
          const double fx=rx*s, fy=ry*s, fz=rz*s;
          taxi[ii]+=fx; tayi[ii]+=fy; tazi[ii]+=fz;
          taxj[jj]-=fx; tayj[jj]-=fy; tazj[jj]-=fz;
        }
      }
      for(int ii=0; ii<Ti; ii++){ ax[i0+ii]+=taxi[ii]; ay[i0+ii]+=tayi[ii]; az[i0+ii]+=tazi[ii]; }
      for(int jj=0; jj<Tj; jj++){ ax[j0+jj]+=taxj[jj]; ay[j0+jj]+=tayj[jj]; az[j0+jj]+=tazj[jj]; }
    }
  }

  long long t0=now_ns();

  for(int s=0;s<STEPS;s++){
    // SIMD Archetype: Vectorized position updates
    simd_update_positions(px, py, pz, vx, vy, vz, ax, ay, az, dt, N);
    for(int i=0;i<N;i++){ ax[i]=ay[i]=az[i]=0.0; }

    for(int i0=0;i0<N;i0+=TILE){
      const int i1=(i0+TILE<N)?(i0+TILE):N;
      for(int i=i0;i<i1;i++){
        for(int j=i+1;j<i1;j++){
          const double rx=px[j]-px[i], ry=py[j]-py[i], rz=pz[j]-pz[i];
          const double r2=rx*rx+ry*ry+rz*rz+eps2;
          const double inv=1.0/sqrt(r2*r2*r2);
          const double s=G*inv;
          const double fx=rx*s, fy=ry*s, fz=rz*s;
          ax[i]+=fx; ay[i]+=fy; az[i]+=fz;
          ax[j]-=fx; ay[j]-=fy; az[j]-=fz;
        }
      }
      for(int j0=i1;j0<N;j0+=TILE){
        const int j1=(j0+TILE<N)?(j0+TILE):N;
        const int Ti=i1-i0, Tj=j1-j0;
        double *taxi=(double*)alloca(sizeof(double)*Ti);
        double *tayi=(double*)alloca(sizeof(double)*Ti);
        double *tazi=(double*)alloca(sizeof(double)*Ti);
        double *taxj=(double*)alloca(sizeof(double)*Tj);
        double *tayj=(double*)alloca(sizeof(double)*Tj);
        double *tazj=(double*)alloca(sizeof(double)*Tj);
        for(int t=0;t<Ti;t++){ taxi[t]=tayi[t]=tazi[t]=0.0; }
        for(int t=0;t<Tj;t++){ taxj[t]=tayj[t]=tazj[t]=0.0; }

        for(int ii=0; ii<Ti; ii++){
          const int i=i0+ii; const double pix=px[i], piy=py[i], piz=pz[i];
          for(int jj=0; jj<Tj; jj++){
            const int j=j0+jj;
            const double rx=px[j]-pix, ry=py[j]-piy, rz=pz[j]-piz;
            const double r2=rx*rx+ry*ry+rz*rz + eps2;
            const double inv=1.0/sqrt(r2*r2*r2);
            const double s=G*inv;
            const double fx=rx*s, fy=ry*s, fz=rz*s;
            taxi[ii]+=fx; tayi[ii]+=fy; tazi[ii]+=fz;
            taxj[jj]-=fx; tayj[jj]-=fy; tazj[jj]-=fz;
          }
        }
        for(int ii=0; ii<Ti; ii++){ ax[i0+ii]+=taxi[ii]; ay[i0+ii]+=tayi[ii]; az[i0+ii]+=tazi[ii]; }
        for(int jj=0; jj<Tj; jj++){ ax[j0+jj]+=taxj[jj]; ay[j0+jj]+=tayj[jj]; az[j0+jj]+=tazj[jj]; }
      }
    }
    // SIMD Archetype: Vectorized velocity updates
    simd_update_velocities(vx, vy, vz, ax, ay, az, dt, N);
  }

  long long t1=now_ns();

  double KE=0.0, PE=0.0;
  for(int i=0;i<N;i++){ KE+=0.5*(vx[i]*vx[i]+vy[i]*vy[i]+vz[i]*vz[i]); }
  for(int i=0;i<N;i++){
    for(int j=i+1;j<N;j++){
      const double rx=px[j]-px[i], ry=py[j]-py[i], rz=pz[j]-pz[i];
      const double r = sqrt(rx*rx+ry*ry+rz*rz + 1e-9);
      PE += -1.0/r;
    }
  }
  const double E=KE+PE;
  printf("TASK=nbody_sym,N=%d,TIME_NS=%lld,ENERGY=%.9f\n", N, (t1-t0), E);

  free(px); free(py); free(pz); free(vx); free(vy); free(vz); free(ax); free(ay); free(az);
  return 0;
}
