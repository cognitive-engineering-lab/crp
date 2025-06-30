# Documentation (e.g., Doxygen)

While C++ has several documentation tools, such as Doxygen and Sphinx, Rust has
a single documentation tool,
[Rustdoc](https://doc.rust-lang.org/rustdoc/index.html). Rustdoc is supported by
[`docs.rs`](https://docs.rs/), cargo,
[rust-analyzer](https://rust-analyzer.github.io/), and is the tool used for
documenting [the standard
library](https://doc.rust-lang.org/std/vec/struct.Vec.html#blanket-implementations).
Rustdoc is installed by default with the Rust toolchain for most distributions.

The features and options available for Rustdoc are documented in the [Rustdoc
Book](https://doc.rust-lang.org/rustdoc/). The book also documents [best
practices for documenting Rust
code](https://doc.rust-lang.org/rustdoc/how-to-write-documentation.html), which
differ slightly from the recommended practices for documenting C++ code using
Doxygen.

The Cargo integration is documented at in the Cargo book under the [`cargo
doc`](https://doc.rust-lang.org/cargo/commands/cargo-doc.html) and [`cargo
rustdoc`](https://doc.rust-lang.org/cargo/commands/cargo-rustdoc.html) commands,
as well as in the [doctests
section](https://doc.rust-lang.org/cargo/commands/cargo-test.html#documentation-tests)
of the `cargo test` command.

This chapter compares some aspects of Rustdoc with Doxygen in order to help with
understanding what to expect when using Rustdoc when coming from Doxygen or
similar C++ documentation tools.

## Output formats

Unlike Doxygen which can also produce PDF and man page output, Rustdoc only
produces HTML output. The produced documentation does include client-side
searching, which includes the ability to search by type signature.

The [rust-analyzer](https://rust-analyzer.github.io/) language server supports
Rustdoc comments, and makes them available to editors with language server
protocol support on hover, even for Rustdoc comments in the current project.

## Rustdoc comment syntax

Unlike Doxygen, which has several supported comment syntaxes for C++, Rustup
supports a single comment syntax. Comments beginning with `//!` document the
top-level module or crate. Comments beginning with `///` document the following
item.

<div class="comparison">

```cpp
/**
 * @file myheader.h
 * @brief A description of this file.
 *
 * A longer description, with examples, etc.
 */

/**
 * @brief A description of this class.
 *
 * A longr description, with examples, etc.
 */
struct MyClass {
  // ...
};
```

```rust
//! A description of this module or crate.
//!
//! A longer description, with examples, etc.

/// A description of this type.
///
/// A longer description, with examples, etc.
struct MyClass {
    // ...
}
```

</div>

### Special forms

The content of the comment up until the first blank line is treated similarly to
the `@brief` form in Doxygen.

Aside from that, Rustdoc does not have special forms for documenting various
parts of an item, such as the parameters of a function. Instead, [Markdown
syntax can be
used](https://doc.rust-lang.org/rustdoc/how-to-write-documentation.html#markdown)
to format the documentation, which is otherwise given in prose.

There are several common conventions used for structuring documentation
comments. The most common convention is to include sections (defined using
Markdown header syntax) for whichever of the following are necessary for the
item being documented:

- panics (for functions that panic, e.g., on
  [`Vec::split_at`](https://doc.rust-lang.org/std/vec/struct.Vec.html#panics-32)),
- safety (for unsafe functions, e.g., on
  [`Vec::split_at_unchecked`](https://doc.rust-lang.org/std/primitive.slice.html#safety-10)),
  and
- examples (e.g., on [`Vec::split_at`](https://doc.rust-lang.org/std/vec/struct.Vec.html#examples-105)).

The following comment compares documentation for a C++ function using Doxygen to
documentation for a Rust function using Rustdoc.

<div class="comparison">
<a id="special-forms-comparison"></a>

```cpp
/**
 * @brief Computes the factorial.
 *
 * Computes the factorial in a stack-safe way.
 * The factorial is defined as...
 *
 * @code
 * #include <cassert>
 * #include "factorial.h"
 *
 * int main() {
 *    int res = factorial(3);
 *    assert(6 == res);
 * }
 * @endcode
 *
 * @param n The number of which to take the factorial
 *
 * @return The factorial
 *
 * @exception domain_error If n < 0
 */
int factorial(int n);
```

```rust
/// Computes the factorial.
///
/// Computes the factorial in a stack-safe way.
/// The factorial is defined as...
///
/// # Examples
///
/// ```
/// let res = factorial(3);
/// assert_eq!(6, res);
/// ```
///
/// # Panics
///
/// Requires that `n >= 0`, otherwise panics.
/// For the non-panicking version see
/// [`factorial_checked`].
fn factorial(n: i32) -> i32 {
    // ...
#    todo!()
}
```

<div>

### Automatic documentation

Many of the things that can be derived from the code are automatically included
by Rustdoc. A major one is that trait implementations (e.g., on
[`Vec`](https://doc.rust-lang.org/std/vec/struct.Vec.html#trait-implementations)),
including blanket implementations (e.g., on
[`Vec`](https://doc.rust-lang.org/std/vec/struct.Vec.html#blanket-implementations)),
for a type do not have to be documented manually because implementations that
are visible in the crate are automatically discovered and included by Rustdoc.

# Additional features

Some valuable Rustdoc features may not be expected by someone coming from using
Doxygen. Because those features provide significant benefit, they are pointed
out here.

## Doctest support via Rustdoc and `cargo test`

One specific benefit of including examples when documenting Rust programs using
Rustdoc is that [the examples can be included in the test suite when running
`cargo
test`](https://doc.rust-lang.org/rustdoc/write-documentation/documentation-tests.html).

The handling of examples as tests in Rustdoc includes logic for handling partial
programs, so that even [the code example in the earlier
comparison](#special-forms-comparison) can serve as a test.

## Local documentation for project and installed libraries

Local documentation for both the working project and dependent libraries can be
viewed in-browser using `cargo doc --open`. Private items for the project can be
included in the documentation by using `cargo doc --open
--document-private-items`. Because Rustdoc comments are also used by the
[rust-analyzer](https://rust-analyzer.github.io/) language server to provide
documentation on hover in compatible editors, it is often worth it to document
private items using Rustdoc comments.
