// Fixed version with same methodology as Tenge
use std::env;
use std::time::Instant;

fn main() {
    let args: Vec<String> = env::args().collect();
    let n: usize = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(90);
    let reps: usize = args.get(2).and_then(|s| s.parse().ok()).unwrap_or(2000000);
    
    let reps = if reps <= 0 { 2000000 } else { reps };

    // Warm-up
    let mut a: u64 = 0;
    let mut b: u64 = 1;
    for _i in 0..n {
        let t = a + b;
        a = b;
        b = t;
    }

    // Clean measurement - same as Tenge
    let mut sink: u64 = 0;  // accumulate result to prevent optimization
    let start = Instant::now();
    for _r in 0..reps {
        let mut a: u64 = 0;
        let mut b: u64 = 1;
        for _i in 0..n {
            let t = a + b;
            a = b;
            b = t;
        }
        sink += b;  // accumulate result, don't just discard
    }
    let end = Instant::now();
    
    // Calculate average time per iteration (same as Tenge)
    let total_time = end.duration_since(start);
    let avg_ns = total_time.as_nanos() / reps as u128;
    
    println!("TASK=fib_iter_fixed,N={},TIME_NS={},SINK={}", n, avg_ns, sink);
}
