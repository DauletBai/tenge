use std::time::Instant;
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    let matrix_size: usize = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(200);
    
    let t0 = Instant::now();
    
    let mut matrix_a = vec![vec![0.0; matrix_size]; matrix_size];
    let mut matrix_b = vec![vec![0.0; matrix_size]; matrix_size];
    let mut matrix_c = vec![vec![0.0; matrix_size]; matrix_size];
    
    for i in 0..matrix_size {
        for j in 0..matrix_size {
            matrix_a[i][j] = (i + j) as f64 * 0.01;
            matrix_b[i][j] = (i as f64 - j as f64) * 0.01;
        }
    }
    
    for i in 0..matrix_size {
        for j in 0..matrix_size {
            let mut sum = 0.0;
            for k in 0..matrix_size {
                sum += matrix_a[i][k] * matrix_b[k][j];
            }
            matrix_c[i][j] = sum;
        }
    }
    
    let mut trace = 0.0;
    for i in 0..matrix_size {
        trace += matrix_c[i][i];
    }
    
    let elapsed = t0.elapsed();
    let t1 = elapsed.as_nanos() as u64;
    println!("TASK=matrix_ops,N={},TIME_NS={},TRACE={:.6}", matrix_size, t1, trace);
}
