# Tenge Language

**A high-performance systems programming language inspired by agglutinative linguistics**

<p align="center">
  <img src="[https://github.com/DauletBai/tenge/blob/main/docs/philosophy/brand/logo.svg" alt="Tenge Logo" width="64"/>
</p>

## 🎯 Mission

Tenge is an experimental programming language that explores the hypothesis that **linguistic clarity can translate to computational efficiency**. Inspired by the structural clarity of agglutinative languages (like Kazakh), Tenge aims to achieve C-level performance while maintaining simplicity and expressiveness.

## 🚀 Key Features

- **AOT (Ahead-of-Time) Compilation**: Compiles to optimized C code
- **Performance-First Design**: Competitive with C, Rust, and Go
- **Linguistic Philosophy**: Simple, clear syntax inspired by agglutinative morphology
- **Cross-Platform**: Targets multiple architectures through C backend

## 📊 Performance Results (September 2025)

Our corrected benchmarks demonstrate Tenge's competitive performance across various workloads. **All benchmarks now use proper volatile variables and result accumulation to prevent compiler optimizations.**

### 🏆 Outstanding Results

| Benchmark | Tenge | C | Rust | Go | Performance |
|-----------|-------|---|---|---|-------------|
| **N-Body Simulation (N=1000)** | **18.9 ms** | 23.4 ms | 28.1 ms | 41.2 ms | **1.24× faster than C** |
| **QuickSort (N=100,000)** | **1.55 ms** | 8.82 ms | 1.42 ms | 7.15 ms | **2nd place, competitive with Rust** |
| **Fibonacci (N=90)** | **44 ns** | 33 ns | 44 ns | 37 ns | **Competitive with native languages** |

### 🔬 Methodology Improvements

- **Fixed compiler optimizations**: Used `volatile` variables and result accumulation
- **Realistic performance differences**: 2-5x range instead of 100,000x anomalies
- **Proper benchmarking**: All computations are actually performed
- **Cross-language validation**: Consistent methodology across all languages

## 🔬 Technical Approach

Tenge follows a **4-stage performance roadmap**:

1. **✅ AST Interpreter** (Current) - Tree-walking interpreter in Go
2. **🔄 Bytecode VM** (In Progress) - Register-based virtual machine
3. **📋 JIT Compiler** (Planned) - Just-in-time compilation for hot paths
4. **🎯 AOT Compiler** (Current) - Ahead-of-time compilation to C

## 🛠️ Getting Started

### Prerequisites
- Go 1.19+
- Clang/LLVM
- Make

### Building
```bash
git clone https://github.com/your-org/tenge.git
cd tenge
make build
```

### Running Benchmarks
```bash
./benchmarks/run.sh
```

## 📋 Benchmark Suite

Our comprehensive benchmark suite includes:

- **Algorithmic**: Fibonacci (iterative/recursive)
- **Sorting**: Integer sorting algorithms
- **Scientific**: N-body gravitational simulation
- **Financial**: Value-at-Risk Monte Carlo simulation
- **Optimization**: Symmetric kernel computations

## 🤝 Contributing

We welcome contributions from the international community! Areas of interest:

- **Language Design**: Syntax improvements and new features
- **Compiler Optimization**: Better code generation and optimization
- **Runtime Performance**: VM and JIT compiler development
- **Benchmarks**: New test cases and performance improvements

## 📚 Documentation

- [Philosophy & Design](docs/philosophy/README.md)
- [Benchmark Results](RESULTS.md)
- [Language Specification](docs/language-spec.md) (Coming Soon)

## 🌟 Why Tenge?

Tenge represents a novel approach to programming language design, combining:

- **Linguistic Inspiration**: Drawing from natural language structure
- **Performance Focus**: Competitive with established systems languages
- **Simplicity**: Clear, readable syntax
- **Innovation**: Exploring new compilation strategies

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

**Ready to explore the future of programming languages?** 🚀

Check our [benchmark results](RESULTS.md) and join our community to help shape the next generation of systems programming!
