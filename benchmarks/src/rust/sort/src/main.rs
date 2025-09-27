use std::env;
use std::time::Instant;

fn main() {
    let mut n: usize = 100000;
    if let Some(arg1) = env::args().nth(1) {
        if let Ok(v) = arg1.parse::<usize>() {
            n = v;
        }
    }
    let batch: usize = env::var("BATCH_ITER").ok().and_then(|v| v.parse().ok()).unwrap_or(0);

    let run = || -> u64 {
        // детерминированный псевдослучайный массив
        let mut x: u64 = 88172645463393265;
        let mut v = vec![0u64; n];
        for i in 0..n {
            // xorshift64*
            x ^= x >> 12; x ^= x << 25; x ^= x >> 27;
            let val = x.wrapping_mul(2685821657736338717);
            v[i] = val;
        }
        v.sort_unstable();
        // checksum
        let mut acc: u64 = 0;
        for i in (0..n).step_by(n/16.max(1)) {
            acc ^= v[i];
        }
        acc
    };

    let iters = if batch > 0 { batch } else { 1 };
    let t0 = Instant::now();
    let mut acc_total: u64 = 0;
    for _ in 0..iters {
        acc_total ^= run();
    }
    let ns = t0.elapsed().as_nanos() as u64;
    println!("{}", ns);
}