# ⚪ Melody: High-Performance Othello AI ⚫

**Melody** is a high-performance Othello (Reversi) engine written in C++23. Designed for competition, it combines the power of classical search algorithms with modern predictive models to deliver top-level gameplay.

---

## 🧠 Melody's Intelligence

Melody's architecture is built on a synergy between cutting-edge search algorithms and fine-grained statistical evaluation:

* Search: Minimax algorithm with Alpha-Beta pruning, enhanced by ProbCut (***Michael Buro***) for selective pruning based on confidence statistics.
* Optimization: Global Transposition Table for position caching and search acceleration.
* Multithreading: Leverages hardware parallelism to optimize performance on modern multi-core architectures (YBWC).

* Evaluation (Patterns & FM): Implementation of a pattern-based evaluation function (***Michael Buro***), augmented with a Factorization Machines layer (***Steffen Rendle***). The "FM" contribution enables modeling of cross-correlations between patterns, where the classical approach relies on a simple linear sum.

---

## 🔌 Connectivity and Protocols
Melody's primary role is to compete against other engines online. It supports:
* IOStd: (Experimental interface) For local testing and analysis.
* GGS (Internet Othello Server): Based on a modernized ODK (Othello Development Kit), enabling stable and fast communication with the GGS game server.

---

## 🛠️ Development and Installation
The engine is specifically optimized for the Apple ecosystem to maximize nodes computed per second.

* Environment: **macOS**
* Architecture: Fully optimized for **Apple Silicon (M-series)** chips. Intensive use of **NEON (ARM)** instructions to accelerate bitboard operations and FM evaluation computations.
* Language: C++ (standard `gnu++23`)
* Tooling: Xcode project (`.xcodeproj`) included.

> [!TIP]
> Compilation note: To fully benefit from ARM/NEON optimizations, always compile in Release mode via Xcode.

---

## 📜 Acknowledgements and Credits

> [!IMPORTANT]
> Heritage and Credits:
> Melody integrates ideas and logic segments from the Edax codebase. Immense respect and thanks to ***Richard Delorme*** and ***Toshihiko Okuhara*** for their invaluable contribution to the community.
> GGS support is based on a modernization of the original ODK by ***C Welty***.
> A warm thank-you to the GGS community for providing such a stimulating environment for exchanging ideas.

---

#### 🚀 Release Technical Details:
This version of Melody is distributed as a Universal Binary, compatible with both ARM and x86-64 architectures.

ARM Architecture: The code is optimized to make intensive use of the NEON (SIMD) extension, ensuring increased performance on compatible processors (Apple Silicon, Raspberry Pi, etc.).

x86-64 Architecture: This version now benefits from hardware optimizations via AVX/AVX2 instruction sets. Core compute-intensive functions have been rewritten using SIMD to maximize data throughput and minimize latency.

💡 Community Feedback & Testing.
While the essential functions are now implemented, feedback regarding performance across different micro-architectures (Intel/AMD) is highly appreciated to help refine our profiling.

If you would like to help implement these optimizations.

If you have the testing capabilities to validate specific builds.

🐧 Linux Compatibility
Unlike macOS, no binary is provided for Linux to avoid dependency issues across distributions.

Procedure: Please download the Source Code and follow the instructions in 🛠️ Compilation.md.

This ensures the binary will be perfectly optimized for your distribution (Ubuntu, Debian, Arch, etc.) and your processor.

#### Feel free to open an Issue or contact me to discuss!
---


## ⚖️ License
Distributed under the MIT License.
