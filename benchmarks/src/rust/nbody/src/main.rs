// FILE: benchmarks/src/rust/nbody/src/main.rs
// Purpose: N-body benchmark (gravity, velocity-Verlet). Deterministic init and timing.

use std::env;
use std::time::Instant;

#[derive(Clone, Copy)]
struct Vec3 {
    x: f64,
    y: f64,
    z: f64,
}
impl Vec3 {
    #[inline]
    fn add(&mut self, b: Vec3) {
        self.x += b.x;
        self.y += b.y;
        self.z += b.z;
    }
    #[inline]
    fn sub(a: Vec3, b: Vec3) -> Vec3 {
        Vec3 {
            x: a.x - b.x,
            y: a.y - b.y,
            z: a.z - b.z,
        }
    }
    #[inline]
    fn smul(a: Vec3, s: f64) -> Vec3 {
        Vec3 {
            x: a.x * s,
            y: a.y * s,
            z: a.z * s,
        }
    }
    #[inline]
    fn dot(a: Vec3, b: Vec3) -> f64 {
        a.x * b.x + a.y * b.y + a.z * b.z
    }
}

// Xoroshiro-style mix (xorshift*)
static mut XS: u64 = 0x9E3779B97F4A7C15;

#[inline]
fn n64() -> u64 {
    unsafe {
        let mut x = XS;
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        XS = x;
        x = x.wrapping_mul(0x2545F4914F6CDD1D);
        x
    }
}

#[inline]
fn u01() -> f64 {
    ((n64() >> 11) as f64) * (1.0 / 9007199254740992.0)
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let n: usize = if args.len() > 1 {
        args[1].parse().unwrap_or(4096)
    } else {
        4096
    };
    let steps: i32 = if args.len() > 2 {
        args[2].parse().unwrap_or(10)
    } else {
        10
    };
    let dt: f64 = if args.len() > 3 {
        args[3].parse().unwrap_or(1e-3)
    } else {
        1e-3
    };

    let mut pos = vec![Vec3 { x: 0.0, y: 0.0, z: 0.0 }; n];
    let mut vel = vec![Vec3 { x: 0.0, y: 0.0, z: 0.0 }; n];
    let mut acc = vec![Vec3 { x: 0.0, y: 0.0, z: 0.0 }; n];
    let mut mass = vec![1.0f64; n];

    unsafe {
        XS = 123456789;
    }
    for i in 0..n {
        pos[i] = Vec3 {
            x: u01(),
            y: u01(),
            z: u01(),
        };
        vel[i] = Vec3 {
            x: (u01() - 0.5) * 1e-3,
            y: (u01() - 0.5) * 1e-3,
            z: (u01() - 0.5) * 1e-3,
        };
        acc[i] = Vec3 { x: 0.0, y: 0.0, z: 0.0 };
        mass[i] = 1.0;
    }

    let g: f64 = 1.0;
    let eps2: f64 = 1e-9;

    // Initial acceleration
    for i in 0..n {
        let mut ai = Vec3 { x: 0.0, y: 0.0, z: 0.0 };
        for j in 0..n {
            if i == j {
                continue;
            }
            let rij = Vec3::sub(pos[j], pos[i]);
            let r2 = Vec3::dot(rij, rij) + eps2;
            let inv = 1.0 / (r2 * r2 * r2).sqrt();
            let s = g * mass[j] * inv;
            ai.add(Vec3::smul(rij, s));
        }
        acc[i] = ai;
    }

    let t0 = Instant::now();
    for _ in 0..steps {
        // Position update
        for i in 0..n {
            let dx = Vec3::smul(vel[i], dt);
            let aa = Vec3::smul(acc[i], 0.5 * dt * dt);
            pos[i].add(dx);
            pos[i].add(aa);
        }
        // Acceleration and velocity update
        for i in 0..n {
            let mut ai = Vec3 { x: 0.0, y: 0.0, z: 0.0 };
            for j in 0..n {
                if i == j {
                    continue;
                }
                let rij = Vec3::sub(pos[j], pos[i]);
                let r2 = Vec3::dot(rij, rij) + eps2;
                let inv = 1.0 / (r2 * r2 * r2).sqrt();
                let s = g * mass[j] * inv;
                ai.add(Vec3::smul(rij, s));
            }
            let mut half = Vec3::smul(acc[i], 0.5 * dt);
            half.add(Vec3::smul(ai, 0.5 * dt));
            vel[i].add(half);
            acc[i] = ai;
        }
    }
    let dt_ns = t0.elapsed().as_nanos() as i128;

    // Energy (diagnostic)
    let mut ke = 0.0f64;
    let mut pe = 0.0f64;
    for i in 0..n {
        ke += 0.5 * mass[i] * Vec3::dot(vel[i], vel[i]);
    }
    for i in 0..n {
        for j in (i + 1)..n {
            let rij = Vec3::sub(pos[j], pos[i]);
            let r = (Vec3::dot(rij, rij) + eps2).sqrt();
            pe += -g * mass[i] * mass[j] / r;
        }
    }
    let e = ke + pe;

    println!("{}", dt_ns);
}