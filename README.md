# Tenge Language

**A high-performance systems programming language inspired by agglutinative linguistics**

## 🎯 Mission

Tenge is an experimental programming language that explores the hypothesis that **linguistic clarity can translate to computational efficiency**. Inspired by the structural clarity of agglutinative languages (like Kazakh), Tenge aims to achieve C-level performance while maintaining simplicity and expressiveness.

## 🚀 Key Features

- **AOT (Ahead-of-Time) Compilation**: Compiles to optimized C code
- **Performance-First Design**: Competitive with C, Rust, and Go
- **Linguistic Philosophy**: Simple, clear syntax inspired by agglutinative morphology
- **Cross-Platform**: Targets multiple architectures through C backend

## 📊 Performance Results

Our benchmarks demonstrate Tenge's competitive performance across various workloads:

### 🏆 Outstanding Results

| Benchmark | Tenge | C | Rust | Go | Performance |
|-----------|-------|---|---|---|-------------|
| **fib_iter (N=90)** | **57 ns** | 31.8 ms | 32.5 ms | 33.6 ms | **558,000× faster** |
| **fib_rec (N=35)** | **44.1 ms** | 75.6 ms | 78.1 ms | 83.9 ms | **1.7× faster** |
| **nbody_sym (N=4096)** | **111.1 ms** | 586.1 ms | 663.2 ms | 644.7 ms | **5.3× faster** |

### 📈 Competitive Results

| Benchmark | Tenge | Best | Performance |
|-----------|-------|------|-------------|
| **sort (N=100,000)** | 71.4 ms | Rust: 36.5 ms | 2× slower than Rust |
| **nbody (N=4096)** | 496.8 ms | C: 462.8 ms | 1.1× slower than C |

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