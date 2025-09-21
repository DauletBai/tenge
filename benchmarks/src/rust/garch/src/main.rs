use std::time::Instant;
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    let n_obs: usize = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(10000);
    
    let omega = 0.0001;
    let alpha = 0.1;
    let beta = 0.85;
    
    let t0 = Instant::now();
    
    let mut returns = vec![0.0; n_obs];
    let mut variances = vec![0.0; n_obs];
    
    for i in 0..n_obs {
        returns[i] = 0.01 * ((i % 100) as f64 - 50.0) / 50.0;
        let var_prev = if i == 0 { omega / (1.0 - alpha - beta) } else { variances[i - 1] };
        variances[i] = omega + alpha * returns[i] * returns[i] + beta * var_prev;
    }
    
    let mut vol_sum = 0.0;
    for i in 0..n_obs {
        vol_sum += variances[i].sqrt();
    }
    
    let elapsed = t0.elapsed();
    let t1 = elapsed.as_nanos() as u64;
    println!("TASK=garch,N={},TIME_NS={},VOL_SUM={:.6}", n_obs, t1, vol_sum);
}
