use std::env;
use std::io::{self, Write};

#[inline(never)]
fn fib_iter(n: u64) -> u64 {
    let (mut a, mut b) = (0u64, 1u64);
    for _ in 0..n {
        let t = a.wrapping_add(b);
        a = b;
        b = t;
    }
    a
}

// Волатильный приёмник: не даём оптимизатору выкинуть работу
static mut SINK_GLOBAL: u64 = 0;

fn main() {
    let mut n: u64 = 90;
    if let Some(arg1) = env::args().nth(1) {
        if let Ok(v) = arg1.parse::<u64>() {
            n = v;
        }
    }
    let inner = env::var("INNER_REPS")
        .ok()
        .and_then(|s| s.parse::<u64>().ok())
        .unwrap_or(0);

    if inner == 0 {
        let v = fib_iter(n);
        unsafe { SINK_GLOBAL ^= v; }
    } else {
        let mut s: u64 = 0;
        for _ in 0..inner {
            s ^= fib_iter(n);
        }
        unsafe { SINK_GLOBAL ^= s; }
    }

    // Без вывода по умолчанию.
    if env::var("PRINT_SINK").ok().as_deref() == Some("1") {
        // печать вне критического участка
        let v = unsafe { SINK_GLOBAL };
        let _ = writeln!(io::stdout(), "{v}");
    }
}