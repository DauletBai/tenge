// benchmarks/src/rust/var_mc_acc/src/main.rs
// Accuracy benchmark for Value-at-Risk (VaR) estimation.

use std::env;
use std::time::Instant;

struct Rng { s: u64 }
impl Rng {
    fn seed(seed: u64) -> Self {
        Self { s: if seed == 0 { 0x123456789ABCDEF0 } else { seed } }
    }
    fn u64(&mut self) -> u64 {
        let mut x = self.s;
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        self.s = x;
        x.wrapping_mul(0x2545F4914F6CDD1D)
    }
    fn uniform(&mut self) -> f64 {
        ((self.u64() >> 11) as f64) * (1.0 / 9007199254740992.0)
    }
}

fn invnorm(p: f64) -> f64 {
    let p = if p <= 0.0 { 1e-16 } else if p >= 1.0 { 1.0 - 1e-16 } else { p };
    // Simple approximation for normal quantile
    -2.0_f64.sqrt() * (2.0 * p).ln().sqrt()
}

fn main() {
    let mut args = env::args().skip(1);
    let n: usize = args.next().and_then(|s| s.parse().ok()).unwrap_or(1_000_000);
    let mu: f64 = args.next().and_then(|s| s.parse().ok()).unwrap_or(0.0);
    let sigma: f64 = args.next().and_then(|s| s.parse().ok()).unwrap_or(1.0);
    let alpha: f64 = args.next().and_then(|s| s.parse().ok()).unwrap_or(0.99);

    let start = Instant::now();
    
    let mut r = Rng::seed(123456789);
    let mut acc = 0.0;
    
    for _ in 0..n {
        let u1 = r.uniform();
        let u2 = r.uniform();
        let r = (-2.0 * u1.ln()).sqrt();
        let z = r * (2.0 * std::f64::consts::PI * u2).cos();
        let pnl = mu + sigma * z;
        acc += pnl;
    }
    
    let q = mu + sigma * invnorm(alpha);
    let elapsed = start.elapsed();
    let time_ns = (elapsed.as_secs() as u128) * 1_000_000_000u128 + (elapsed.subsec_nanos() as u128);
    
    println!("TASK=var_mc_acc,N={},TIME_NS={},ALPHA={:.6},VAR={:.12},ACC={:.6}",
             n, time_ns, alpha, q, acc / (n as f64));
}
