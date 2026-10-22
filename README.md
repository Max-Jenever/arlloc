# arlloc

High-performance arena memory allocator (bump allocator) with a C core
and a safe Rust wrapper.
Provides O(1) memory allocation without:
fragmentation, locks, or system calls during operation.

## Project Architecture

The project is a single Rust crate in which the low-level logic
is implemented in C (C11 standard) and transparently compiled via build.rs
using the cc crate. The Rust wrapper encapsulates the unsafe FFI calls,
ensuring correct lifetime management and automatic resource release (Drop).

## Installation

*Run the following command in the root directory of your Rust project*

```bash

cargo add arlloc

```

*Or*

*Manually add the dependency to the [dependencies] section in the Cargo.toml file*

```toml
[dependencies]
arlloc = "0.1.1"
```

## Build and testing

A C compiler (GCC/Clang) and an up-to-date Rust toolchain are required.
All stages are managed through Cargo.

```bash

cargo build --release


cargo test


cargo bench
```

## Using (Rust)

```rust
use arlloc::Arena;

fn main() {
    // Initialization arena (size 64 KB)
    let arena = Arena::new(65536, 65536).expect("Error memory allocation");

    // allocation memory (type will be release `Copy` trait)
    if let Some(val) = arena.alloc(67u64) {
        assert_eq!(*val, 67);
    }

    // Reset arena
    arena.reset();
} 
// Arena was be dropped ,memory is free
```

## Benchmarks

Comparative benchmarks were performed using the criterion framework (Rust) and std::chrono (C++).
Test workload: allocation of a 64-byte structure.

| allocation | singl allocation | mass allocation (10 000 iteration) | note |
| :--- | :--- | :--- | :--- |
| **arlloc (C/Rust FFI)** | **~6.68 ns** | **~65.0 µs** | Linear complexity O(1), no checks in the hot path. |
| bumpalo (Rust) | ~16.1 ns | ~163.0 µs | Overhead from bounds checks and UnsafeCell. |
| std::pmr (C++17) | ~18.4 ns | ~46.1 µs* | *DCE artifact when compiling with -O3. |

*Conclusion: The raw allocation speed of arlloc exceeds its counterparts
by 58–64% due to minimizing instructions in the critical path and the
absence of indirect calls (vtable).*

## Docs

Full reference documentation for the C API (Doxygen) and the Rust wrapper (rustdoc) is available at the link:

[https://max-jenever.github.io/arlloc/](https://max-jenever.github.io/arlloc/)

## License

The project is distributed under a dual license:
MIT or Apache-2.0
Details are provided in the LICENSE-MIT and LICENSE-APACHE
files in the repository root directory.
