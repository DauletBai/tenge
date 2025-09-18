// FILE: benchmarks/src/rust/nbody_sym/src/main.rs
// Purpose: Symmetric N-body (i<j) with SoA + tiling + temp buffers.

use std::time::Instant;

#[inline]
fn now_ns() -> u128 {
    Instant::now().elapsed().as_nanos() // overwritten below by precise measure
}

#[derive(Clone, Copy)]
struct Rng(u64);
impl Rng {
    #[inline] fn new(seed: u64) -> Self { Self(seed) }
    #[inline] fn next_u64(&mut self) -> u64 {
        let mut x = self.0;
        x ^= x >> 12; x ^= x << 25; x ^= x >> 27;
        self.0 = x;
        x.wrapping_mul(0x2545F4914F6CDD1Du64)
    }
    #[inline] fn u01(&mut self) -> f64 {
        ((self.next_u64() >> 11) as f64) * (1.0 / 9007199254740992.0)
    }
}

fn main() {
    let mut args = std::env::args().skip(1);
    let n: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(4096);
    let steps: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(10);
    let dt: f64 = args.next().and_then(|s| s.parse().ok()).unwrap_or(1e-3);

    let mut px = vec![0.0f64; n];
    let mut py = vec![0.0f64; n];
    let mut pz = vec![0.0f64; n];
    let mut vx = vec![0.0f64; n];
    let mut vy = vec![0.0f64; n];
    let mut vz = vec![0.0f64; n];
    let mut ax = vec![0.0f64; n];
    let mut ay = vec![0.0f64; n];
    let mut az = vec![0.0f64; n];

    let mut rng = Rng::new(123456789);
    for i in 0..n {
        px[i] = rng.u01(); py[i] = rng.u01(); pz[i] = rng.u01();
        vx[i] = (rng.u01() - 0.5) * 1e-3;
        vy[i] = (rng.u01() - 0.5) * 1e-3;
        vz[i] = (rng.u01() - 0.5) * 1e-3;
        ax[i] = 0.0; ay[i] = 0.0; az[i] = 0.0;
    }

    let g: f64 = 1.0;
    let eps2: f64 = 1e-9;
    let tile: usize = 64;

    // initial accel
    for i0 in (0..n).step_by(tile) {
        let i1 = (i0 + tile).min(n);
        // intra
        for i in i0..i1 {
            for j in (i+1)..i1 {
                let rx = px[j]-px[i];
                let ry = py[j]-py[i];
                let rz = pz[j]-pz[i];
                let r2 = rx*rx+ry*ry+rz*rz + eps2;
                let inv = 1.0 / (r2*r2*r2).sqrt();
                let s = g*inv;
                let fx = rx*s; let fy=ry*s; let fz=rz*s;
                ax[i]+=fx; ay[i]+=fy; az[i]+=fz;
                ax[j]-=fx; ay[j]-=fy; az[j]-=fz;
            }
        }
        // inter
        for j0 in (i1..n).step_by(tile) {
            let j1 = (j0 + tile).min(n);
            let ti = i1 - i0;
            let tj = j1 - j0;
            let mut taxi = vec![0.0f64; ti];
            let mut tayi = vec![0.0f64; ti];
            let mut tazi = vec![0.0f64; ti];
            let mut taxj = vec![0.0f64; tj];
            let mut tayj = vec![0.0f64; tj];
            let mut tazj = vec![0.0f64; tj];

            for (ii, i) in (i0..i1).enumerate() {
                let pix = px[i]; let piy = py[i]; let piz = pz[i];
                for (jj, j) in (j0..j1).enumerate() {
                    let rx = px[j]-pix; let ry=py[j]-piy; let rz=pz[j]-piz;
                    let r2 = rx*rx+ry*ry+rz*rz + eps2;
                    let inv = 1.0 / (r2*r2*r2).sqrt();
                    let s = g*inv;
                    let fx = rx*s; let fy=ry*s; let fz=rz*s;
                    taxi[ii]+=fx; tayi[ii]+=fy; tazi[ii]+=fz;
                    taxj[jj]-=fx; tayj[jj]-=fy; tazj[jj]-=fz;
                }
            }
            for ii in 0..ti { ax[i0+ii]+=taxi[ii]; ay[i0+ii]+=tayi[ii]; az[i0+ii]+=tazi[ii]; }
            for jj in 0..tj { ax[j0+jj]+=taxj[jj]; ay[j0+jj]+=tayj[jj]; az[j0+jj]+=tazj[jj]; }
        }
    }

    let t0 = Instant::now();
    for _ in 0..steps {
        for i in 0..n {
            px[i] += vx[i]*dt + 0.5*ax[i]*dt*dt;
            py[i] += vy[i]*dt + 0.5*ay[i]*dt*dt;
            pz[i] += vz[i]*dt + 0.5*az[i]*dt*dt;
        }
        for i in 0..n { ax[i]=0.0; ay[i]=0.0; az[i]=0.0; }

        for i0 in (0..n).step_by(tile) {
            let i1 = (i0 + tile).min(n);
            for i in i0..i1 {
                for j in (i+1)..i1 {
                    let rx = px[j]-px[i];
                    let ry = py[j]-py[i];
                    let rz = pz[j]-pz[i];
                    let r2 = rx*rx+ry*ry+rz*rz + eps2;
                    let inv = 1.0 / (r2*r2*r2).sqrt();
                    let s = g*inv;
                    let fx = rx*s; let fy=ry*s; let fz=rz*s;
                    ax[i]+=fx; ay[i]+=fy; az[i]+=fz;
                    ax[j]-=fx; ay[j]-=fy; az[j]-=fz;
                }
            }
            for j0 in (i1..n).step_by(tile) {
                let j1 = (j0 + tile).min(n);
                let ti = i1 - i0;
                let tj = j1 - j0;
                let mut taxi = vec![0.0f64; ti];
                let mut tayi = vec![0.0f64; ti];
                let mut tazi = vec![0.0f64; ti];
                let mut taxj = vec![0.0f64; tj];
                let mut tayj = vec![0.0f64; tj];
                let mut tazj = vec![0.0f64; tj];

                for (ii, i) in (i0..i1).enumerate() {
                    let pix = px[i]; let piy = py[i]; let piz = pz[i];
                    for (jj, j) in (j0..j1).enumerate() {
                        let rx = px[j]-pix; let ry=py[j]-piy; let rz=pz[j]-piz;
                        let r2 = rx*rx+ry*ry+rz*rz + eps2;
                        let inv = 1.0 / (r2*r2*r2).sqrt();
                        let s = g*inv;
                        let fx = rx*s; let fy=ry*s; let fz=rz*s;
                        taxi[ii]+=fx; tayi[ii]+=fy; tazi[ii]+=fz;
                        taxj[jj]-=fx; tayj[jj]-=fy; tazj[jj]-=fz;
                    }
                }
                for ii in 0..ti { ax[i0+ii]+=taxi[ii]; ay[i0+ii]+=tayi[ii]; az[i0+ii]+=tazi[ii]; }
                for jj in 0..tj { ax[j0+jj]+=taxj[jj]; ay[j0+jj]+=tayj[jj]; az[j0+jj]+=tazj[jj]; }
            }
        }
        for i in 0..n {
            vx[i] += ax[i]*dt; vy[i] += ay[i]*dt; vz[i] += az[i]*dt;
        }
    }
    let t1 = t0.elapsed().as_nanos() as u128;

    let mut ke = 0.0f64;
    for i in 0..n { ke += 0.5*(vx[i]*vx[i]+vy[i]*vy[i]+vz[i]*vz[i]); }
    let mut pe = 0.0f64;
    for i in 0..n {
        for j in (i+1)..n {
            let rx = px[j]-px[i];
            let ry = py[j]-py[i];
            let rz = pz[j]-pz[i];
            let r = (rx*rx+ry*ry+rz*rz + 1e-9).sqrt();
            pe += -1.0/r;
        }
    }
    let e = ke + pe;
    println!("TASK=nbody_sym,N={},TIME_NS={},ENERGY={:.9}", n, t1, e);
}