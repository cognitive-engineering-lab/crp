# Libraries

C++ programs tend to either use libraries that come with operating system
distributions or that are vendored.

Rust programs tend to rely on a central registry of Rust libraries ("crates")
called [crates.io](https://crates.io/) (along with a central documentation
repository created from the in-code documentation of those crates called
[docs.rs](https://docs.rs/)). Dependencies on crates are managed using the
[Cargo package manager](https://doc.rust-lang.org/cargo/index.html).

[Lib.rs](https://lib.rs/) is a good resource for finding popular crates organized by category.

## Some specific alternatives

| C++ library                   | Rust alternative                                                        |
|-------------------------------|-------------------------------------------------------------------------|
| STL UTF-16 and UTF-32 strings | [widestring](https://docs.rs/widestring/latest/widestring/)             |
| STL random                    | [rand](https://github.com/rust-random/rand)                             |
| STL regex                     | [regex](https://github.com/rust-lang/regex)                             |
| Reflection                    | [bevy\_reflect](https://docs.rs/bevy_reflect/latest/bevy_reflect/)      |
| Boost.Test                    | [cargo test](https://doc.rust-lang.org/book/ch11-01-writing-tests.html) |
| pybind11                      | [PyO3](https://pyo3.rs/)                                                |
| OpenSSL                       | [rustls](https://github.com/rustls/rustls)                              |
<!-- | STL `multiset`                |                                                             |
| STL `multimap`                |                                                             | -->

If there is a C++ library that you use where you cannot find a Rust alternative,
please leave feedback using the link below, letting us know the name and purpose
of the library.

## Supply chain management

In situations where managing the library supply chain is important, Cargo can be
used either with [custom self-managed or organization-managed
registries](https://doc.rust-lang.org/cargo/reference/registries.html) or with
[vendored versions of dependencies fetched from
crates.io](https://doc.rust-lang.org/cargo/commands/cargo-vendor.html).

Both approaches provide mechanisms for reviewing dependencies as part supply
chain security.

Solutions for supply chain security that do not involve vendoring or custom
registries are [in progress](https://github.com/rust-lang/rfcs/pull/3724).
