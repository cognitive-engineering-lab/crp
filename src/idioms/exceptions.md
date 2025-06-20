# Exceptions and error handling

In C++ errors that are to be handled by the caller are sometimes indicated by
sentinel values (e.g., `std::map::find` producing an empty iterator), sometimes
indicated by exceptions (e.g., `std::vector::at` throwing `std::out_of_range`),
and sometimes indicated by setting an error bit (e.g., `std::fstream::fail`).
Errors that are not intended to be handled by the caller are usually indicated
by exceptions (e.g., `std::bad_cast`). Errors that are due to programming bugs
often just result in undefined behavior (e.g., `std::vector::operator[]` when
the index is out-of-bounds).

In contrast, safe Rust has two mechanisms for indicating errors. When the error
is expected to be handled by the caller (because it is due to, e.g., user
input), the function returns a
[`Result`](https://doc.rust-lang.org/std/result/index.html) or
[`Option`](https://doc.rust-lang.org/std/option/index.html). When the error is
due to a programming bug, the function panics. Undefined behavior can only occur
if unchecked variants of functions are used with unsafe Rust.

Some libraries in Rust will offer two versions of an API, one which returns a
`Result` or `Option` type and one of which panics, so that the interpretation of
the error (expected exceptional case or programmer bug) can be chosen by the
caller.

The major differences between using `Result` or `Option` and using exceptions
are that

1. `Result` and `Option` force explicit handling of the error case in order to
   access the contained value. This also differs from `std::expected` in C++23.
2. When propagating errors with `Result`, the types of the errors must match.
   There are libraries for making this easier to handle.

## `Result` vs `Option`

The approaches demonstrated in the Rust examples in this chapter apply to both
`Result` and `Option`. When the type is `Option` it indicates that there is no
additional information to provide in the error case: `Option::None` does not
contain a value, but `Result::Err` does. When there is no additional
information, is usually because there is exactly one circumstance which can
cause the error case.

It is possible to convert between the two types.

```rust
fn main() {
    let r: Result<i32, &'static str> =
        None.ok_or("my error message");
    let r2: Result<i32, &'static str> =
        None.ok_or_else(|| "expensive error message");
    let o: Option<i32> = r.ok();
}
```
