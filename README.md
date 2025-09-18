# Tenge Language Benchmarks

Tenge is an experimental systems language focused on **performance, clarity, and fairness of comparison**.  
Our mission is to explore compilation and runtime strategies that allow Tenge to reach or exceed C-level performance, while remaining simple and portable.

This repository contains benchmark results against **C, Rust, and Go** on a set of classical workloads:

- **fib_iter / fib_rec** — micro-benchmarks for iteration/recursion overhead.
- **sort** — 100k integer sort.
- **var_mc** — Value-at-Risk Monte Carlo simulation (with Tenge-specific optimizations).
- **nbody / nbody_sym** — gravitational N-body simulation (classical + symmetric kernels).

## Highlights
- **Tenge ≈ C** on core workloads (`sort`, `nbody`, `nbody_sym`) within ~1–2%.
- **Tenge leads** in VaR Monte Carlo with O(N) quickselect (~23.5 ms vs C/Go ~120 ms and Rust ~55 ms).
- **Microbenchmarks:** `fib_iter(90)` shows Tenge in the tens of ns range, much faster than Go, slightly faster than C/Rust.
- **Symmetric N-body:** Tenge ~31.7 ms, C ~32.2 ms, while Rust/Go are ~0.3 s.

See [RESULTS.md](RESULTS.md) for full tables and details.
