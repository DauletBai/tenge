// FILE: benchmarks/src/go/nbody/main.go
package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
	"time"
)

type vec3 struct{ x, y, z float64 }

func vadd(a *vec3, b vec3) { a.x += b.x; a.y += b.y; a.z += b.z }
func vsub(a, b vec3) vec3  { return vec3{a.x - b.x, a.y - b.y, a.z - b.z} }
func smul(a vec3, s float64) vec3 { return vec3{a.x * s, a.y * s, a.z * s} }
func dot(a, b vec3) float64 { return a.x*b.x + a.y*b.y + a.z*b.z }

var xs uint64 = 0x9E3779B97F4A7C15
func n64() uint64 { x:=xs; x^=x>>12; x^=x<<25; x^=x>>27; xs=x; return x*0x2545F4914F6CDD1D }
func u01() float64 { return float64(n64()>>11) * (1.0/9007199254740992.0) }

func main() {
	N := 4096
	steps := 10
	dt := 1e-3
	if len(os.Args) > 1 { if v,err:=strconv.Atoi(os.Args[1]); err==nil { N=v } }
	if len(os.Args) > 2 { if v,err:=strconv.Atoi(os.Args[2]); err==nil { steps=v } }
	if len(os.Args) > 3 { if v,err:=strconv.ParseFloat(os.Args[3],64); err==nil { dt=v } }

	pos := make([]vec3, N)
	vel := make([]vec3, N)
	acc := make([]vec3, N)
	m   := make([]float64, N)

	xs = 123456789
	for i:=0;i<N;i++{
		pos[i] = vec3{u01(), u01(), u01()}
		vel[i] = vec3{(u01()-0.5)*1e-3, (u01()-0.5)*1e-3, (u01()-0.5)*1e-3}
		acc[i] = vec3{}
		m[i]   = 1.0
	}

	const G = 1.0
	const eps2 = 1e-9

	for i:=0;i<N;i++{
		var ai vec3
		for j:=0;j<N;j++{
			if i==j { continue }
			rij := vsub(pos[j], pos[i])
			r2 := dot(rij,rij)+eps2
			inv := 1.0/math.Sqrt(r2*r2*r2)
			s := G*m[j]*inv
			vadd(&ai, smul(rij,s))
		}
		acc[i]=ai
	}

	t0 := time.Now()
	for s:=0; s<steps; s++{
		for i:=0;i<N;i++{
			dx := smul(vel[i], dt)
			aa := smul(acc[i], 0.5*dt*dt)
			vadd(&pos[i], dx); vadd(&pos[i], aa)
		}
		for i:=0;i<N;i++{
			var ai vec3
			for j:=0;j<N;j++{
				if i==j { continue }
				rij := vsub(pos[j], pos[i])
				r2 := dot(rij,rij)+eps2
				inv := 1.0/math.Sqrt(r2*r2*r2)
				s := G*m[j]*inv
				vadd(&ai, smul(rij,s))
			}
			half := smul(acc[i], 0.5*dt)
			vadd(&half, smul(ai, 0.5*dt))
			vadd(&vel[i], half)
			acc[i]=ai
		}
	}
	t1 := time.Since(t0)

	KE := 0.0; PE := 0.0
	for i:=0;i<N;i++{ KE += 0.5*m[i]*dot(vel[i],vel[i]) }
	for i:=0;i<N;i++{
		for j:=i+1;j<N;j++{
			rij := vsub(pos[j],pos[i])
			r := math.Sqrt(dot(rij,rij)+eps2)
			PE += -G*m[i]*m[j]/r
		}
	}
	E := KE+PE

	fmt.Printf("TASK=nbody,N=%d,TIME_NS=%d,ENERGY=%.9f\n", N, t1.Nanoseconds(), E)
}