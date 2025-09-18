#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
static inline long long now_ns(){struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return (long long)ts.tv_sec*1000000000LL+ts.tv_nsec;}
static void msort(int* a,int n,int* tmp){ if(n<=1) return; int m=n/2; msort(a,m,tmp); msort(a+m,n-m,tmp); int i=0,j=m,k=0; while(i<m && j<n){ tmp[k++]= (a[i]<=a[j])?a[i++]:a[j++]; } while(i<m) tmp[k++]=a[i++]; while(j<n) tmp[k++]=a[j++]; memcpy(a,tmp,sizeof(int)*n); }
int main(int argc,char**argv){int n=(argc>1)?atoi(argv[1]):100000; int*arr=(int*)malloc(sizeof(int)*n); int*tmp=(int*)malloc(sizeof(int)*n); for(int i=0;i<n;i++){arr[i]=n-i;} long long t0=now_ns(); msort(arr,n,tmp); long long t1=now_ns(); printf("TASK=sort_msort,N=%d,TIME_NS=%lld\n",n,(t1-t0)); free(arr); free(tmp); return 0;}
