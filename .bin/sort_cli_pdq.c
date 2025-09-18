#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
static inline long long now_ns(){struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return (long long)ts.tv_sec*1000000000LL+ts.tv_nsec;}
static inline void iswap(int* a,int* b){int t=*a;*a=*b;*b=t;}
static inline int median3(int a,int b,int c){ if(a<b){ if(b<c) return b; return (a<c)?c:a; } else { if(a<c) return a; return (b<c)?c:b; } }
static void insertion(int* a,int n){ for(int i=1;i<n;i++){ int x=a[i],j=i-1; while(j>=0 && a[j]>x){ a[j+1]=a[j]; j--; } a[j+1]=x; } }
static int part_range(int* a,int l,int r){
    int m = l + ((r-l)>>1);
    int p = median3(a[l], a[m], a[r]);
    int i=l, j=r;
    while(i<=j){
        while(a[i]<p) i++;
        while(a[j]>p) j--;
        if(i<=j){ iswap(&a[i],&a[j]); i++; j--; }
    }
    return i;
}
static void heapify(int* a,int n,int i){ for(;;){ int L=i*2+1, R=L+1, big=i; if(L<n && a[L]>a[big]) big=L; if(R<n && a[R]>a[big]) big=R; if(big==i) break; iswap(&a[i],&a[big]); i=big; } }
static void hs_heap_sort(int* a,int n){ for(int i=n/2-1;i>=0;i--) heapify(a,n,i); for(int i=n-1;i>0;i--){ iswap(&a[0],&a[i]); heapify(a,i,0); } }
static void introsort_range(int* a,int l,int r,int depth){
    const int CUT=24;
    while(l<r){
        int n = r-l+1;
        if(n<=CUT){ insertion(a+l, n); return; }
        if(depth==0){ hs_heap_sort(a+l, n); return; }
        int p = part_range(a,l,r);
        if(p-l < r-(p-1)){ introsort_range(a,l,p-1,depth-1); l=p; }
        else { introsort_range(a,p,r,depth-1); r=p-1; }
    }
}
static void sort_pdq(int* a,int n){ int depth = (int)(2.0 * floor(log((double)n)/log(2.0))); introsort_range(a,0,n-1,depth); }
int main(int argc,char**argv){
    int n=(argc>1)?atoi(argv[1]):100000;
    int*arr=(int*)malloc(sizeof(int)*n);
    for(int i=0;i<n;i++){ arr[i]=n-i; }
    long long t0=now_ns();
    sort_pdq(arr,n);
    long long t1=now_ns();
    printf("TASK=sort_pdq,N=%d,TIME_NS=%lld\n",n,(t1-t0));
    free(arr);
    return 0;
}
