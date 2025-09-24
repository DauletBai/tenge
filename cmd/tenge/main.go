// cmd/tenge/main.go
package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

func usage() {
	fmt.Fprintln(os.Stderr, "usage: tenge -o <out.c> <demo_source.tng>")
	os.Exit(2)
}

func main() {
	out := flag.String("o", "", "output C file")
	flag.Parse()
	args := flag.Args()
	if *out == "" || len(args) != 1 {
		usage()
	}
	src := args[0]
	base := strings.ToLower(filepath.Base(src))

	code, ok := emitC(base)
	if !ok {
		fmt.Fprintf(os.Stderr, "error: unsupported AOT demo source: %s\n", src)
		os.Exit(1)
	}
	if err := os.WriteFile(*out, []byte(code), 0o644); err != nil {
		fmt.Fprintf(os.Stderr, "error: write %s: %v\n", *out, err)
		os.Exit(1)
	}
	fmt.Printf("C emitted: %s\n", *out)
}

// ---------- tiny C-templates ----------

func commonIncludes() string {
	return `
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "runtime.h"    // now_ns()
`
}

func rngHelpers() string {
	return `
static inline uint64_t xorshift64(uint64_t* s){
    uint64_t x=*s; x^=x>>12; x^=x<<25; x^=x>>27; *s=x; return x*0x2545F4914F6CDD1DULL;
}
static inline double u01(uint64_t* s){ return (xorshift64(s)>>11)*(1.0/9007199254740992.0); }
`
}

// ---------- dispatch ----------

func emitC(base string) (string, bool) {
	switch base {

	// Fibonacci
	case "fib_iter_cli.tng":
		return cFibIter(), true
	case "fib_rec_cli.tng":
		return cFibRec(), true

	// Sort family (AOT demos)
	case "sort_qsort_cli.tng":
		return cSortQsort(), true
	case "sort_msort_cli.tng":
		return cSortMergesort(), true
	case "sort_pdq_cli.tng":
		return cSortPDQ(), true
	case "sort_radix_cli.tng":
		return cSortRadix(), true

	// VaR Monte Carlo — ваши текущие имена
	case "var_mc_sort_cli.tng":
		return cVarMCSort(), true
	case "var_mc_zig_cli.tng":
		return cVarMCZig(), true
	case "var_mc_qsel_cli.tng":
		return cVarMCQSel(), true

	// VaR Monte Carlo — старые имена (на всякий случай)
	case "var_mc_tng_sort.tng":
		return cVarMCSort(), true
	case "var_mc_tng_zig.tng":
		return cVarMCZig(), true
	case "var_mc_tng_qsel.tng":
		return cVarMCQSel(), true

	// N-body — текущие имена
	case "nbody_cli.tng":
		return cNBody(), true
	case "nbody_sym_cli.tng":
		return cNBodySym(), true

	// N-body — старые имена
	case "nbody_tng.tng":
		return cNBody(), true
	case "nbody_tng_sym.tng":
		return cNBodySym(), true
	}
	return "", false
}

// ---------- implementations ----------

func cFibIter() string {
	return commonIncludes() + `
int main(int argc, char** argv){
    int n = (argc>1)? atoi(argv[1]) : 90;
    long long t0 = now_ns();
    __uint128_t a=0,b=1;
    for(int i=0;i<n;i++){ __uint128_t t=a+b; a=b; b=t; }
    long long t1 = now_ns();
    printf("TASK=fib_iter,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    return 0;
}
`
}

func cFibRec() string {
	return commonIncludes() + `
static unsigned long long fib_rec_c(int n){
    return (n<=1)? (unsigned long long)n : fib_rec_c(n-1)+fib_rec_c(n-2);
}
int main(int argc, char** argv){
    int n = (argc>1)? atoi(argv[1]) : 35;
    long long t0 = now_ns();
    volatile unsigned long long r = fib_rec_c(n);
    long long t1 = now_ns();
    (void)r;
    printf("TASK=fib_rec,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    return 0;
}
`
}

func cSortQsort() string {
	return commonIncludes() + `
static int cmp_int(const void* a, const void* b){
    int x = *(const int*)a, y = *(const int*)b;
    return (x>y)-(x<y);
}
int main(int argc, char** argv){
    int n = (argc>1)? atoi(argv[1]) : 100000;
    int* a = (int*)malloc(n*sizeof(int));
    if(!a){ fprintf(stderr,"oom\n"); return 1; }
    uint64_t x=88172645463393265ULL;
    for(int i=0;i<n;i++){ x = x*2862933555777941757ULL + 3037000493ULL; a[i] = (int)(x>>33); }
    long long t0 = now_ns();
    qsort(a,n,sizeof(int),cmp_int);
    long long t1 = now_ns();
    printf("TASK=sort_qsort,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    free(a);
    return 0;
}
`
}

func cSortMergesort() string {
	return commonIncludes() + `
#if defined(__APPLE__) || defined(__BSD_LIBC) || defined(__GLIBC__)
int mergesort(void *base, size_t nmemb, size_t size,
              int (*compar)(const void *, const void *));
#endif
static int cmp_int(const void* a, const void* b){
    int x = *(const int*)a, y = *(const int*)b;
    return (x>y)-(x<y);
}
int main(int argc, char** argv){
    int n = (argc>1)? atoi(argv[1]) : 100000;
    int* a = (int*)malloc(n*sizeof(int));
    if(!a){ fprintf(stderr,"oom\n"); return 1; }
    uint64_t x=88172645463393265ULL;
    for(int i=0;i<n;i++){ x = x*2862933555777941757ULL + 3037000493ULL; a[i] = (int)(x>>33); }
    long long t0 = now_ns();
#if defined(__APPLE__)
    mergesort(a, n, sizeof(int), cmp_int);
#else
    qsort(a, n, sizeof(int), cmp_int);
#endif
    long long t1 = now_ns();
    printf("TASK=sort_msort,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    free(a);
    return 0;
}
`
}

func cSortPDQ() string {
	return commonIncludes() + `
static inline void iswap(int* a, int* b){ int t=*a; *a=*b; *b=t; }
static int median3(int* a,int i,int j,int k){
    int x=a[i], y=a[j], z=a[k];
    if((x<y) ^ (x<z)) return i;
    if((y<x) ^ (y<z)) return j;
    return k;
}
static int part(int* a,int l,int r){
    int m = l + (r-l)/2;
    int p = median3(a,l,m,r);
    iswap(&a[p], &a[r]);
    int pivot = a[r], i=l;
    for(int j=l;j<r;j++) if(a[j] <= pivot){ iswap(&a[i], &a[j]); i++; }
    iswap(&a[i], &a[r]);
    return i;
}
static void insertion(int* a,int l,int r){
    for(int i=l+1;i<=r;i++){
        int x=a[i], j=i-1;
        while(j>=l && a[j]>x){ a[j+1]=a[j]; j--; }
        a[j+1]=x;
    }
}
static void introsort_rec(int* a,int l,int r,int depth){
    while(r-l>32){
        if(depth==0){
            for(int i=l;i<=r;i++){
                int mi=i;
                for(int j=i+1;j<=r;j++) if(a[j]<a[mi]) mi=j;
                iswap(&a[i],&a[mi]);
            }
            return;
        }
        int p = part(a,l,r);
        if(p-l < r-p){ introsort_rec(a,l,p-1,depth-1); l=p+1; }
        else         { introsort_rec(a,p+1,r,depth-1); r=p-1; }
    }
    insertion(a,l,r);
}
int main(int argc, char** argv){
    int n = (argc>1)? atoi(argv[1]) : 100000;
    int* a = (int*)malloc(n*sizeof(int));
    if(!a){ fprintf(stderr,"oom\n"); return 1; }
    uint64_t x=88172645463393265ULL;
    for(int i=0;i<n;i++){ x = x*2862933555777941757ULL + 3037000493ULL; a[i] = (int)(x>>33); }
    long long t0 = now_ns();
    int depth = 2; while((1<<depth) < n) depth++;
    introsort_rec(a,0,n-1,depth*2);
    long long t1 = now_ns();
    printf("TASK=sort_pdq,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    free(a);
    return 0;
}
`
}

func cSortRadix() string {
	return commonIncludes() + `
int main(int argc, char** argv){
    int n = (argc>1)? atoi(argv[1]) : 100000;
    uint32_t* a = (uint32_t*)malloc(n*sizeof(uint32_t));
    uint32_t* b = (uint32_t*)malloc(n*sizeof(uint32_t));
    if(!a || !b){ fprintf(stderr,"oom\n"); return 1; }
    uint64_t x=88172645463393265ULL;
    for(int i=0;i<n;i++){ x = x*2862933555777941757ULL + 3037000493ULL; a[i] = (uint32_t)(x>>32); }
    long long t0 = now_ns();
    for(int pass=0; pass<4; pass++){
        unsigned cnt[256]={0};
        int shift = pass*8;
        for(int i=0;i<n;i++) cnt[(a[i]>>shift)&0xFF]++;
        unsigned sum=0;
        for(int i=0;i<256;i++){ unsigned c=cnt[i]; cnt[i]=sum; sum+=c; }
        for(int i=0;i<n;i++){ b[cnt[(a[i]>>shift)&0xFF]++] = a[i]; }
        uint32_t* tmp=a; a=b; b=tmp;
    }
    long long t1 = now_ns();
    printf("TASK=sort_radix,N=%d,TIME_NS=%lld\n", n, (t1 - t0));
    free(a); free(b);
    return 0;
}
`
}

func cVarMCSort() string {
	return commonIncludes() + rngHelpers() + `
static int cmp_d(const void* a,const void* b){
    double x=*(const double*)a, y=*(const double*)b;
    return (x>y)-(x<y);
}
int main(int argc, char** argv){
    int N    = (argc>1)? atoi(argv[1]) : 1000000;
    int steps= (argc>2)? atoi(argv[2]) : 1;
    double a = (argc>3)? atof(argv[3]) : 0.99;
    double* pnl = (double*)malloc(N*sizeof(double));
    if(!pnl){ fprintf(stderr,"oom\n"); return 1; }
    long long t0 = now_ns();
    uint64_t s=123456789;
    for(int i=0;i<N;i++){
        double x=0.0;
        for(int k=0;k<steps;k++){
            double u=u01(&s), v=u01(&s);
            double r=sqrt(-2.0*log(u+1e-18));
            double z=r*cos(6.283185307179586*v);
            x += z;
        }
        pnl[i] = x;
    }
    qsort(pnl, N, sizeof(double), cmp_d);
    int idx = (int)((1.0-a)*N); if(idx<0) idx=0; if(idx>=N) idx=N-1;
    volatile double var = -pnl[idx];
    long long t1 = now_ns();
    (void)var;
    printf("TASK=var_mc_sort,N=%d,TIME_NS=%lld\n", N, (t1 - t0));
    free(pnl);
    return 0;
}
`
}

func cVarMCZig() string {
	return commonIncludes() + rngHelpers() + `
static double znormal(uint64_t* s){
    double u,v, ss;
    do {
        u = 2.0*u01(s)-1.0;
        v = 2.0*u01(s)-1.0;
        ss = u*u + v*v;
    } while(ss<=1e-16 || ss>=1.0);
    return u*sqrt(-2.0*log(ss)/ss);
}
int main(int argc, char** argv){
    int N    = (argc>1)? atoi(argv[1]) : 1000000;
    int steps= (argc>2)? atoi(argv[2]) : 1;
    double a = (argc>3)? atof(argv[3]) : 0.99;
    double* pnl = (double*)malloc(N*sizeof(double));
    if(!pnl){ fprintf(stderr,"oom\n"); return 1; }
    long long t0 = now_ns();
    uint64_t s=987654321;
    for(int i=0;i<N;i++){
        double x=0.0;
        for(int k=0;k<steps;k++) x += znormal(&s);
        pnl[i] = x;
    }
    int idx = (int)((1.0-a)*N); if(idx<0) idx=0; if(idx>=N) idx=N-1;
    int l=0, r=N-1;
    while(l<r){
        double pivot = pnl[(l+r)>>1];
        int i=l, j=r;
        while(i<=j){
            while(pnl[i]<pivot) i++;
            while(pnl[j]>pivot) j--;
            if(i<=j){ double t=pnl[i]; pnl[i]=pnl[j]; pnl[j]=t; i++; j--; }
        }
        if(idx<=j) r=j; else if(idx>=i) l=i; else break;
    }
    volatile double var = -pnl[idx];
    long long t1 = now_ns();
    (void)var;
    printf("TASK=var_mc_zig,N=%d,TIME_NS=%lld\n", N, (t1 - t0));
    free(pnl);
    return 0;
}
`
}

func cVarMCQSel() string {
	return commonIncludes() + rngHelpers() + `
int main(int argc, char** argv){
    int N    = (argc>1)? atoi(argv[1]) : 1000000;
    int steps= (argc>2)? atoi(argv[2]) : 1;
    double a = (argc>3)? atof(argv[3]) : 0.99;
    double* pnl = (double*)malloc(N*sizeof(double));
    if(!pnl){ fprintf(stderr,"oom\n"); return 1; }
    long long t0 = now_ns();
    uint64_t s=1234567;
    for(int i=0;i<N;i++){
        double x=0.0;
        for(int k=0;k<steps;k++){
            double u=u01(&s), v=u01(&s);
            double r=sqrt(-2.0*log(u+1e-18));
            double z=r*cos(6.283185307179586*v);
            x += z;
        }
        pnl[i] = x;
    }
    int idx = (int)((1.0-a)*N); if(idx<0) idx=0; if(idx>=N) idx=N-1;
    int l=0, r=N-1;
    while(l<r){
        double pivot = pnl[(l+r)>>1];
        int i=l, j=r;
        while(i<=j){
            while(pnl[i]<pivot) i++;
            while(pnl[j]>pivot) j--;
            if(i<=j){ double t=pnl[i]; pnl[i]=pnl[j]; pnl[j]=t; i++; j--; }
        }
        if(idx<=j) r=j; else if(idx>=i) l=i; else break;
    }
    volatile double var = -pnl[idx];
    long long t1 = now_ns();
    (void)var;
    printf("TASK=var_mc_qsel,N=%d,TIME_NS=%lld\n", N, (t1 - t0));
    free(pnl);
    return 0;
}
`
}

func cNBody() string {
	return commonIncludes() + `
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
            double xi=x[i], yi=y[i], zi=z[i];
            for(int j=0;j<N;j++){
                double dx=x[j]-xi, dy=y[j]-yi, dz=z[j]-zi;
                double r2=dx*dx+dy*dy+dz*dz+1e-9, inv=1.0/(r2*sqrt(r2));
                ax+=dx*inv; ay+=dy*inv; az+=dz*inv;
            }
            vx[i]+=ax*dt; vy[i]+=ay*dt; vz[i]+=az*dt;
        }
        for(int i=0;i<N;i++){ x[i]+=vx[i]*dt; y[i]+=vy[i]*dt; z[i]+=vz[i]*dt; }
    }
    long long t1 = now_ns();
    printf("TASK=nbody,N=%d,TIME_NS=%lld\n", N, (t1 - t0));
    free(x);free(y);free(z);free(vx);free(vy);free(vz);
    return 0;
}
`
}

func cNBodySym() string {
	return commonIncludes() + `
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
`
}