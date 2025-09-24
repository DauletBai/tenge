
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "runtime.h"    // now_ns()

int main(int argc, char** argv){
    int N     = (argc>1)? atoi(argv[1]) : 4096;
    int steps = (argc>2)? atoi(argv[2]) : 10;
    double dt = (argc>3)? atof(argv[3]) : 0.001;
    double* x = (double*)malloc(N*sizeof(double));
    double* y = (double*)malloc(N*sizeof(double));
    double* z = (double*)malloc(N*sizeof(double));
    double* vx= (double*)malloc(N*sizeof(double));
    double* vy= (double*)malloc(N*sizeof(double));
    double* vz= (double*)malloc(N*sizeof(double));
    if(!x||!y||!z||!vx||!vy||!vz){ fprintf(stderr,"oom\n"); return 1; }
    uint64_t s=1;
    for(int i=0;i<N;i++){
        s = s*2862933555777941757ULL + 3037000493ULL; x[i]=(double)((s>>20)&1023)/1024.0;
        s = s*2862933555777941757ULL + 3037000493ULL; y[i]=(double)((s>>20)&1023)/1024.0;
        s = s*2862933555777941757ULL + 3037000493ULL; z[i]=(double)((s>>20)&1023)/1024.0;
        vx[i]=vy[i]=vz[i]=0.0;
    }
    long long t0 = now_ns();
    for(int t=0;t<steps;t++){
        for(int i=0;i<N;i++){
            double ax=0, ay=0, az=0;
            for(int j=i+1;j<N;j++){
                double dx=x[j]-x[i], dy=y[j]-y[i], dz=z[j]-z[i];
                double r2=dx*dx+dy*dy+dz*dz+1e-9, inv=1.0/(r2*sqrt(r2));
                double fx=dx*inv, fy=dy*inv, fz=dz*inv;
                ax+=fx; ay+=fy; az+=fz;
                vx[j]-=fx*dt; vy[j]-=fy*dt; vz[j]-=fz*dt;
            }
            vx[i]+=ax*dt; vy[i]+=ay*dt; vz[i]+=az*dt;
        }
        for(int i=0;i<N;i++){ x[i]+=vx[i]*dt; y[i]+=vy[i]*dt; z[i]+=vz[i]*dt; }
    }
    long long t1 = now_ns();
    printf("TASK=nbody_sym,N=%d,TIME_NS=%lld\n", N, (t1 - t0));
    free(x);free(y);free(z);free(vx);free(vy);free(vz);
    return 0;
}
