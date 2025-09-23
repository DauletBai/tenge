// FILE: benchmarks/src/go/nbody_sym/main.go
package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
	"time"
)

type rng struct{ x uint64 }

func (r *rng) next() uint64 {
	x := r.x
	x ^= x >> 12
	x ^= x << 25
	x ^= x >> 27
	r.x = x
	return x * 0x2545F4914F6CDD1D
}
func (r *rng) u01() float64 {
	return float64(r.next()>>11) * (1.0 / 9007199254740992.0)
}

func main() {
	N := 4096
	STEPS := 10
	dt := 1e-3
	if len(os.Args) > 1 {
		if v, err := strconv.Atoi(os.Args[1]); err == nil { N = v }
	}
	if len(os.Args) > 2 {
		if v, err := strconv.Atoi(os.Args[2]); err == nil { STEPS = v }
	}
	if len(os.Args) > 3 {
		if v, err := strconv.ParseFloat(os.Args[3], 64); err == nil { dt = v }
	}

	px := make([]float64, N)
	py := make([]float64, N)
	pz := make([]float64, N)
	vx := make([]float64, N)
	vy := make([]float64, N)
	vz := make([]float64, N)
	ax := make([]float64, N)
	ay := make([]float64, N)
	az := make([]float64, N)

	r := rng{0x075BCD15}
	for i := 0; i < N; i++ {
		px[i] = r.u01(); py[i] = r.u01(); pz[i] = r.u01()
		vx[i] = (r.u01() - 0.5) * 1e-3
		vy[i] = (r.u01() - 0.5) * 1e-3
		vz[i] = (r.u01() - 0.5) * 1e-3
		ax[i] = 0; ay[i] = 0; az[i] = 0
	}

	const G = 1.0
	const eps2 = 1e-9
	const TILE = 64

	// initial accel
	for i := 0; i < N; i++ { ax[i], ay[i], az[i] = 0,0,0 }
	for i0 := 0; i0 < N; i0 += TILE {
		i1 := i0 + TILE; if i1 > N { i1 = N }
		for i := i0; i < i1; i++ {
			for j := i+1; j < i1; j++ {
				rx := px[j]-px[i]; ry := py[j]-py[i]; rz := pz[j]-pz[i]
				r2 := rx*rx+ry*ry+rz*rz + eps2
				inv := 1.0 / math.Sqrt(r2*r2*r2)
				s := G*inv
				fx, fy, fz := rx*s, ry*s, rz*s
				ax[i]+=fx; ay[i]+=fy; az[i]+=fz
				ax[j]-=fx; ay[j]-=fy; az[j]-=fz
			}
		}
		for j0 := i1; j0 < N; j0 += TILE {
			j1 := j0 + TILE; if j1 > N { j1 = N }
			Ti := i1 - i0; Tj := j1 - j0
			taxi := make([]float64, Ti)
			tayi := make([]float64, Ti)
			tazi := make([]float64, Ti)
			taxj := make([]float64, Tj)
			tayj := make([]float64, Tj)
			tazj := make([]float64, Tj)
			for ii := 0; ii < Ti; ii++ {
				pix, piy, piz := px[i0+ii], py[i0+ii], pz[i0+ii]
				for jj := 0; jj < Tj; jj++ {
					j := j0+jj
					rx := px[j]-pix; ry := py[j]-piy; rz := pz[j]-piz
					r2 := rx*rx+ry*ry+rz*rz + eps2
					inv := 1.0 / math.Sqrt(r2*r2*r2)
					s := G*inv
					fx, fy, fz := rx*s, ry*s, rz*s
					taxi[ii]+=fx; tayi[ii]+=fy; tazi[ii]+=fz
					taxj[jj]-=fx; tayj[jj]-=fy; tazj[jj]-=fz
				}
			}
			for ii := 0; ii < Ti; ii++ { ax[i0+ii]+=taxi[ii]; ay[i0+ii]+=tayi[ii]; az[i0+ii]+=tazi[ii] }
			for jj := 0; jj < Tj; jj++ { ax[j0+jj]+=taxj[jj]; ay[j0+jj]+=tayj[jj]; az[j0+jj]+=tazj[jj] }
		}
	}

	t0 := time.Now()
	for s := 0; s < STEPS; s++ {
		for i := 0; i < N; i++ {
			px[i] += vx[i]*dt + 0.5*ax[i]*dt*dt
			py[i] += vy[i]*dt + 0.5*ay[i]*dt*dt
			pz[i] += vz[i]*dt + 0.5*az[i]*dt*dt
		}
		for i := 0; i < N; i++ { ax[i], ay[i], az[i] = 0,0,0 }

		for i0 := 0; i0 < N; i0 += TILE {
			i1 := i0 + TILE; if i1 > N { i1 = N }
			for i := i0; i < i1; i++ {
				for j := i+1; j < i1; j++ {
					rx := px[j]-px[i]; ry := py[j]-py[i]; rz := pz[j]-pz[i]
					r2 := rx*rx+ry*ry+rz*rz + eps2
					inv := 1.0 / math.Sqrt(r2*r2*r2)
					s := G*inv
					fx, fy, fz := rx*s, ry*s, rz*s
					ax[i]+=fx; ay[i]+=fy; az[i]+=fz
					ax[j]-=fx; ay[j]-=fy; az[j]-=fz
				}
			}
			for j0 := i1; j0 < N; j0 += TILE {
				j1 := j0 + TILE; if j1 > N { j1 = N }
				Ti := i1 - i0; Tj := j1 - j0
				taxi := make([]float64, Ti)
				tayi := make([]float64, Ti)
				tazi := make([]float64, Ti)
				taxj := make([]float64, Tj)
				tayj := make([]float64, Tj)
				tazj := make([]float64, Tj)
				for ii := 0; ii < Ti; ii++ {
					pix, piy, piz := px[i0+ii], py[i0+ii], pz[i0+ii]
					for jj := 0; jj < Tj; jj++ {
						j := j0+jj
						rx := px[j]-pix; ry := py[j]-piy; rz := pz[j]-piz
						r2 := rx*rx+ry*ry+rz*rz + eps2
						inv := 1.0 / math.Sqrt(r2*r2*r2)
						s := G*inv
						fx, fy, fz := rx*s, ry*s, rz*s
						taxi[ii]+=fx; tayi[ii]+=fy; tazi[ii]+=fz
						taxj[jj]-=fx; tayj[jj]-=fy; tazj[jj]-=fz
					}
				}
				for ii := 0; ii < Ti; ii++ { ax[i0+ii]+=taxi[ii]; ay[i0+ii]+=tayi[ii]; az[i0+ii]+=tazi[ii] }
				for jj := 0; jj < Tj; jj++ { ax[j0+jj]+=taxj[jj]; ay[j0+jj]+=tayj[jj]; az[j0+jj]+=tazj[jj] }
			}
		}
		for i := 0; i < N; i++ {
			vx[i] += ax[i]*dt; vy[i] += ay[i]*dt; vz[i] += az[i]*dt
		}
	}
	t1 := time.Since(t0).Nanoseconds()

	ke := 0.0
	for i := 0; i < N; i++ { ke += 0.5*(vx[i]*vx[i]+vy[i]*vy[i]+vz[i]*vz[i]) }
	pe := 0.0
	for i := 0; i < N; i++ {
		for j := i+1; j < N; j++ {
			rx := px[j]-px[i]; ry := py[j]-py[i]; rz := pz[j]-pz[i]
			r := math.Sqrt(rx*rx+ry*ry+rz*rz + 1e-9)
			pe += -1.0/r
		}
	}
	fmt.Printf("TASK=nbody_sym,N=%d,TIME_NS=%d,ENERGY=%.9f\n", N, t1, ke+pe)
}