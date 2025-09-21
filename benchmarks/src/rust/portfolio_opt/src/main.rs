use std::time::Instant;
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    let n_assets: usize = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(100);
    
    let t0 = Instant::now();
    
    let mut returns = vec![0.0; n_assets];
    let mut cov_matrix = vec![vec![0.0; n_assets]; n_assets];
    let mut weights = vec![0.0; n_assets];
    
    for i in 0..n_assets {
        returns[i] = 0.01 + 0.02 * ((i % 10) as f64) / 10.0;
    }
    
    for i in 0..n_assets {
        for j in 0..n_assets {
            cov_matrix[i][j] = if i == j { 0.04 } else { 0.01 * (i + j) as f64 / (2.0 * n_assets as f64) };
        }
    }
    
    for i in 0..n_assets {
        weights[i] = 1.0 / n_assets as f64;
    }
    
    let mut portfolio_var = 0.0;
    for i in 0..n_assets {
        for j in 0..n_assets {
            portfolio_var += weights[i] * weights[j] * cov_matrix[i][j];
        }
    }
    
    let elapsed = t0.elapsed();
    let t1 = elapsed.as_nanos() as u64;
    println!("TASK=portfolio_opt,N={},TIME_NS={},PORTFOLIO_VAR={:.6}", n_assets, t1, portfolio_var);
}
