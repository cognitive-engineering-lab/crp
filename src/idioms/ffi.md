# Rust and C++ interoperability (FFI)

The Rustonomicon [contains a
chapter](https://doc.rust-lang.org/nomicon/ffi.html) covering many of the
concerns relevant to a C++ programmer that wants to call C (or C++ via `extern
"C"` functions) from Rust or Rust from C or C++ code.

Many C libraries have existing crates, both with low-level bindings and with
high-level safe Rust abstractions. For example, for the libgit2 library there is
both a low-level [libgit2-sys crate](https://crates.io/crates/libgit2-sys) and a
high-level [git2 crate](https://crates.io/crates/git2).

Bindings to libraries can be generated from a C header file using
[`bindgen`](https://rust-lang.github.io/rust-bindgen/).
