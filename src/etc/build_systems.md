# Build systems (e.g., CMake)

One major difference between the C++ and Rust ecosystems is that C++ and C
libraries tend to be either provided by OS distributions or be included in the
repository for a project, while Rust has a central language-specific package
registry called [crates.io](https://crates.io/).

This difference is amplified by the fact that the Rust build tool, Cargo, has a
build in package manager that works with crates.io, private registries, local
packages, and vendored sources.

Cargo is documented in detail in the [Cargo
Book](https://doc.rust-lang.org/cargo/).

## Packages for C and C++ system libraries

Many C libraries have crates on crates.io providing both low-level bindings and
high-level safe Rust abstractions. For example, for the libgit2 library there is
both a low-level [libgit2-sys crate](https://crates.io/crates/libgit2-sys) and a
high-level [git2 crate](https://crates.io/crates/git2). See the [chapter on the
Rust FFI](../idioms/ffi.md) for more information on how to define these crates.

## Building C, C++, and Rust code

Cargo [build
scripts](https://doc.rust-lang.org/cargo/reference/build-script-examples.html)
can be used to build C and C++ code as part of a Rust project. The linked
chapter of the Cargo book includes links to resources handling the compilation
of C, C++, and other code, working with `pkg-config`, etc.

## Testing (CTest)

Cargo includes support for [running
tests](https://doc.rust-lang.org/cargo/guide/tests.html).

## Packaging for distribution (CPack)

Unlike CPack which is provided with CMake, Cargo does not come with tools for
packaging for distribution to end users. However, there are third party Cargo
helpers for packaging, such as [cargo-deb](https://crates.io/crates/cargo-deb)
for creating Debian package,
[cargo-generate-rpm](https://crates.io/crates/cargo-generate-rpm) for creating
RPM packages, and [cargo-wix](https://crates.io/crates/cargo-wix) for creating
Windows installers.
