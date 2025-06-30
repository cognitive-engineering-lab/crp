# Pointer-to-implementation (PIMPL)

The [PIMPL pattern](https://en.cppreference.com/w/cpp/language/pimpl.html) in
C++ is usually used for the purpose of improving compilation times by removing
implementation details from the ABI of a translation unit. It also can be used
to hide implementation details that otherwise would be exposed in a header file.

In Rust, the unit of separate compilation is the crate, rather than the file or
module. Within a crate, the compiler minimizes compilation times via incremental
compilation, rather than via separate compilation. Between crates, there is no
guarantee of Rust-native ABI stability, so if an upstream crate changes,
downstream crates need to be recompiled. Thus, for performance purposes, the
PIMPL pattern does not apply.

For the hiding of implementation details, [instead of excluding details from a
header file, modules can be used to control
visibility](../idioms/encapsulation.md).
