# ⚪ Melody: High-Performance Othello AI ⚫

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE.md)

**Melody** is a high-performance Othello (Reversi) engine written in C++23. Designed for competition, it combines the power of classical search algorithms with modern predictive models to deliver top-level playing strength.

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
The engine was originally optimized for the Apple ecosystem to maximize nodes computed per second, and now also builds natively on Windows and Linux.

* Environment: **macOS**, **Windows**, **Linux**
* Architecture: Fully optimized for **Apple Silicon (M-series)** chips. Intensive use of **NEON (ARM)** instructions to accelerate bitboard operations and FM evaluation computations. x86-64 builds (Windows/Linux) leverage **AVX2** for the same operations.
* Language: C++ (standard `C++23` on macOS/Linux, `/std:c++23` on MSVC)
* Tooling:
  * macOS: Xcode project (`.xcodeproj`) included.
  * Linux: GNU `Makefile`.
  * Windows: native MSVC build via `Makefile.win` (NMake).

> [!TIP]
> Compilation note: To fully benefit from ARM/NEON or AVX2 optimizations, always compile in Release mode (Xcode Release scheme, or `make`/`nmake` as documented in [COMPILATION.md](COMPILATION.md)).

---

## 📜 Acknowledgements and Credits

> [!IMPORTANT]
> Heritage and Credits:
> Melody integrates ideas and logic segments from the Edax codebase. Immense respect and thanks to ***Richard Delorme*** and ***Toshihiko Okuhara*** for their invaluable contribution to the community.
> GGS support is based on a modernization of the original ODK by ***C Welty***.
> A warm thank-you to the GGS community for providing such a stimulating environment for exchanging ideas.

---

#### 🚀 Release Technical Details:
This version of Melody is compatible with both ARM NEON and x86-64 AVX2 architectures.

ARM Architecture: The code is optimized to make intensive use of the NEON (SIMD) extension, ensuring increased performance on compatible processors (Apple Silicon, Raspberry Pi 4+, etc.).

x86-64 Architecture: This version now benefits from hardware optimizations via AVX/AVX2 instruction sets. Core compute-intensive functions have been rewritten using SIMD to maximize data throughput and minimize latency.

> [!NOTE]
> 💡 Feedback on performance across different x86-64 micro-architectures (Intel/AMD) is highly appreciated. Feel free to open an Issue or contact me to discuss!

### 🐧 Linux

No binary is provided for Linux to avoid dependency issues across distributions. Please download the source code and follow the instructions in [Compilation.md](COMPILATION.md) to build a binary perfectly optimized for your distribution and processor.

### 🪟 Windows

No pre-compiled binary is provided for Windows. Please download the source code and build it natively with MSVC — see [Compilation.md](COMPILATION.md) for the full procedure (`nmake /f Makefile.win`).

---


## ⚖️ License
This project is licensed under the **GPLv3** - see the [LICENSE](LICENSE.md) file for details.
(Previously licensed under MIT).
