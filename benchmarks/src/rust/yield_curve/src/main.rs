use std::time::Instant;
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    let n_points: usize = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(1000);
    
    let beta0 = 0.05;
    let beta1 = -0.02;
    let beta2 = 0.01;
    let tau = 2.0;
    
    let t0 = Instant::now();
    let mut sum = 0.0;
    
    for i in 0..n_points {
        let t = (i + 1) as f64 * 0.1;
        let yield_val = beta0 + beta1 * (1.0 - (-t / tau).exp()) / (t / tau) + 
                       beta2 * ((1.0 - (-t / tau).exp()) / (t / tau) - (-t / tau).exp());
        sum += yield_val;
    }
    
    let elapsed = t0.elapsed();
    let t1 = elapsed.as_nanos() as u64;
    println!("TASK=yield_curve,N={},TIME_NS={},SUM={:.6}", n_points, t1, sum);
}
