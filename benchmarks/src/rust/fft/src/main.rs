use std::time::Instant;
use std::env;
use std::f64::consts::PI;

fn main() {
    let args: Vec<String> = env::args().collect();
    let n_points: usize = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(1024);
    
    let t0 = Instant::now();
    
    let mut signal = vec![0.0; n_points];
    let mut fft_real = vec![0.0; n_points];
    let mut fft_imag = vec![0.0; n_points];
    
    for i in 0..n_points {
        let t = i as f64 * 2.0 * PI / n_points as f64;
        signal[i] = t.sin() + 0.5 * (3.0 * t).sin() + 0.25 * (5.0 * t).sin();
    }
    
    for k in 0..n_points {
        let mut real_sum = 0.0;
        let mut imag_sum = 0.0;
        for n in 0..n_points {
            let angle = -2.0 * PI * k as f64 * n as f64 / n_points as f64;
            real_sum += signal[n] * angle.cos();
            imag_sum += signal[n] * angle.sin();
        }
        fft_real[k] = real_sum;
        fft_imag[k] = imag_sum;
    }
    
    let mut power_sum = 0.0;
    for i in 0..n_points {
        let power = fft_real[i] * fft_real[i] + fft_imag[i] * fft_imag[i];
        power_sum += power;
    }
    
    let elapsed = t0.elapsed();
    let t1 = elapsed.as_nanos() as u64;
    println!("TASK=fft,N={},TIME_NS={},POWER_SUM={:.6}", n_points, t1, power_sum);
}
