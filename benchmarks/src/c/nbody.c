// FILE: benchmarks/src/c/nbody.c
// Purpose: N-body benchmark (gravity, velocity-Verlet). Deterministic init and timing.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct { double x,y,z; } vec3;

static inline long long now_ns(){
  struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec*1000000000LL + ts.tv_nsec;
}

static inline void vadd(vec3* a, vec3 b){ a->x+=b.x; a->y+=b.y; a->z+=b.z; }
static inline vec3 vsub(vec3 a, vec3 b){ vec3 r={a.x-b.x,a.y-b.y,a.z-b.z}; return r; }
static inline vec3 smul(vec3 a, double s){ vec3 r={a.x*s,a.y*s,a.z*s}; return r; }
static inline double dot(vec3 a, vec3 b){ return a.x*b.x + a.y*b.y + a.z*b.z; }

static unsigned long long xs = 0x9E3779B97F4A7C15ULL;
static inline unsigned long long n64(){
  unsigned long long x=xs; x^=x>>12; x^=x<<25; x^=x>>27; xs=x; return x*0x2545F4914F6CDD1DULL;
}
static inline double u01(){
  return (double)(n64()>>11) * (1.0/9007199254740992.0);
}

int main(int argc, char** argv){
  int N    = (argc>1)?atoi(argv[1]):4096;
  int step = (argc>2)?atoi(argv[2]):10;
  double dt= (argc>3)?atof(argv[3]):1e-3;

  vec3* pos = (vec3*)malloc(sizeof(vec3)*N);
  vec3* vel = (vec3*)malloc(sizeof(vec3)*N);
  vec3* acc = (vec3*)malloc(sizeof(vec3)*N);
  double* m = (double*)malloc(sizeof(double)*N);

  // deterministic init in unit cube, mild velocities
  xs = 123456789ULL;
  for(int i=0;i<N;i++){
    pos[i].x = u01(); pos[i].y = u01(); pos[i].z = u01();
    vel[i].x = (u01()-0.5)*1e-3; vel[i].y = (u01()-0.5)*1e-3; vel[i].z = (u01()-0.5)*1e-3;
    acc[i].x = acc[i].y = acc[i].z = 0.0;
    m[i] = 1.0; // unit masses
  }

  const double G = 1.0;
  const double eps2 = 1e-9; // softening

  // initial acceleration
  for(int i=0;i<N;i++){
    vec3 ai={0,0,0};
    for(int j=0;j<N;j++){
      if(i==j) continue;
      vec3 rij = vsub(pos[j], pos[i]);
      double r2 = dot(rij,rij) + eps2;
      double inv = 1.0/sqrt(r2*r2*r2);
      double s = G*m[j]*inv;
      vadd(&ai, smul(rij,s));
    }
    acc[i]=ai;
  }

  long long t0 = now_ns();
  // velocity-Verlet steps
  for(int s=0; s<step; s++){
    // x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt^2
    for(int i=0;i<N;i++){
      vec3 dx = smul(vel[i],dt);
      vec3 aa = smul(acc[i], 0.5*dt*dt);
      vadd(&pos[i], dx); vadd(&pos[i], aa);
    }
    // a(t+dt)
    for(int i=0;i<N;i++){
      vec3 ai={0,0,0};
      for(int j=0;j<N;j++){
        if(i==j) continue;
        vec3 rij = vsub(pos[j], pos[i]);
        double r2 = dot(rij,rij) + eps2;
        double inv = 1.0/sqrt(r2*r2*r2);
        double sF = G*m[j]*inv;
        vadd(&ai, smul(rij,sF));
      }
      // v(t+dt) = v(t) + 0.5*(a(t)+a(t+dt))*dt
      vec3 half = smul(acc[i], 0.5*dt);
      vadd(&half, smul(ai, 0.5*dt));
      vadd(&vel[i], half);
      acc[i]=ai;
    }
  }
  long long t1 = now_ns();

  // energy (diagnostic)
  double KE=0, PE=0;
  for(int i=0;i<N;i++){
    KE += 0.5*m[i]*dot(vel[i],vel[i]);
  }
  for(int i=0;i<N;i++){
    for(int j=i+1;j<N;j++){
      vec3 rij=vsub(pos[j],pos[i]);
      double r = sqrt(dot(rij,rij)+eps2);
      PE += -G*m[i]*m[j]/r;
    }
  }
  double E = KE+PE;

  printf("TASK=nbody,N=%d,TIME_NS=%lld,ENERGY=%.9f\n", N, (t1-t0), E);

  free(pos); free(vel); free(acc); free(m);
  return 0;
}