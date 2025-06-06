# Expected errors

In C++, `throw` both produces an error (the thrown exception) and initiates
non-local control flow (unwinding to the nearest `catch` block). In Rust, error
values (`Option::None` or `Result::Err`) are returned as normal values from a
function. Rust's `return` statement can be used to return early from a function.

<div class="comparison">

```cpp
#include <stdexcept>

double divide(double dividend, double divisor) {
  if (divisor == 0.0) {
    throw std::domain_error("zero divisor");
  }

  return dividend / divisor;
}
```

```rust
fn divide(
    dividend: f64,
    divisor: f64,
) -> Option<f64> {
    if divisor == 0.0 {
        return None;
    }

    Some(dividend / divisor)
}
```

</div>

The requirement to have the return type indicate that an error is possible means
that callbacks that are permitted to have errors need to be given an `Option` or
`Result` return type. Omitting that is like requiring callbacks to be `noexcept`
in C++. Functions that do not need to indicate errors but that will be used as
callbacks where errors are permitted will need to wrap their results in
`Option::Some` or `Result::Ok`.

<div class="comparison">

```cpp
#include <stdexcept>

int produce_42() {
  return 42;
}

int fail() {
  throw std::runtime_error("oops");
}

int useCallback(int (*func)(void)) {
  return func();
}

int main() {
  try {
    int x = useCallback(produce_42);
    int y = useCallback(fail);

    // use x and y
  } catch (std::runtime_error &e) {
    // handle error
  }
}
```

```rust
fn produce_42() -> i32 {
    42
}

fn fail() -> Option<i32> {
    None
}

fn use_callback(
    f: impl Fn() -> Option<i32>,
) -> Option<i32> {
    f()
}

fn main() {
    // need to wrap produce_42 to match the
    // expected type
    let Some(x) =
        use_callback(|| Some(produce_42()))
    else {
        // handle error
        return;
    };
    let Some(y) = use_callback(fail) else {
        // handle error
        return;
    };
    // use x and y
}
```

</div>

## Handling errors

In C++, the only way to handle exceptions is `catch`. In Rust, all of the
features for dealing with [tagged
unions](../data_modeling/tagged_unions.md) can be used with `Result` and
`Option`. The most approach depends on the intention of the program.

The basic way of handling an error indicated by a `Result` in Rust is by using
`match`.

Using `match` is the most general approach, because it enables handling
additional cases explicitly and can be used as an expression. `match` connotes
equal importance of all branches.

<div class="comparison">

```cpp
#include <vector>
#include <stdexcept>

int main() {
    std::vector<int> v;
    // ... populate v ...
    try {
        auto x = v.at(0);
        // use x
    } catch (std::out_of_range &e) {
        // handle error
    }
}
```

```rust
fn main() {
    let mut v = Vec::<i32>::new();
    // ... populate v ...
    match v.get(0) {
        Some(x) => {
            // use x
        }
        None => {
            // handle error
        }
    }
}
```

</div>

Because handling only a single variant of a Rust enum is so common, the `if let`
syntax support that use case. The syntax both makes it clear that only the one
case is important and reduces the levels of indentation.

`if let` is less general than `match`. It can also be used as an expression, but
can only distinguish one case from the rest. `if let` connotes that the `else`
case is not the normal case, but that some default handling will occur or some
default value will be produced.

Note that with `Result`, `if let` does not enable accessing the error value.

```rust
fn main() {
    let mut v = Vec::<i32>::new();
    // ... populate v ...
    if let Some(x) = v.get(0) {
        // use x
    } else {
        // handle error
    }
}
```

When the error handling involves some kind of control flow operation, like
`break` or `return`, the `let else` syntax is even more concise.

Much like normal `let` statements, `let else` statements can only be used where
statements are expected. `let else` statements also connote that the else case
is not the normal case, and that no further (normal) processing will occur.

```rust
fn main() {
    let mut v = Vec::<i32>::new();
    // ... populate v ...
    let Some(x) = v.get(0) else {
        // handle error
        return;
    };
    // use x
}
```

`Result` and `Option` also have some helper methods for handling errors.
These methods resemble the methods on `std::expected` in C++.

<div class="comparison">

```cpp
#include <expected>
#include <string>

int main() {
  std::expected<int, std::string> res(42);
  auto x(res.transform([](int n) { return n * 2; }));
}
```

```rust
fn main() {
    let res: Result<i32, String> = Ok(42);
    let x = res.map(|n| n * 2);
}
```

</div>

These helper methods and others are described in detail in the documentation for
[`Option`](https://doc.rust-lang.org/std/option/enum.Option.html#implementations)
and
[`Result`](https://doc.rust-lang.org/std/result/enum.Result.html#implementations).

## Borrowed results

In the above examples, the successful results are borrowed from the vector. It
common to need to clone or copy the result into an owned copy, and to want to do
so without having to match on and reconstruct the value. `Result` and `Option`
have helper methods for these purposes.

```rust
fn main() {
    let mut v = Vec::<i32>::new();
    v.push(42);
    let x: Option<&i32> = v.get(0);
    let y: Option<i32> = v.get(0).copied();

    let mut w = Vec::<String>::new();
    w.push("hello".to_string());
    let s: Option<&String> = w.get(0);
    let r: Option<String> = w.get(0).cloned();
}
```

## Propagating errors

In C++, exceptions propagate automatically. In Rust, errors indicated by
`Result` or `Option` must be explicitly propagated. The `?` operator is a
convenience for this. There are also several methods for manipulating `Result`
and `Option` that have a similar effect to propagating the error.

<div class="comparison">

```cpp
#include <cstddef>
#include <vector>

int accessValue(std::vector<std::size_t> indices,
                 std::vector<int> values,
                 std::size_t i) {
  // vector::at throws
  size_t idx(indices.at(i));
  // vector::at throws
  return values.at(idx);
}
```

```rust
fn access_value(
    indices: Vec<usize>,
    values: Vec<i32>,
    i: usize,
) -> Option<i32> {
    // * dereferences the &i32 to copy it
    // ? propagates the None
    let idx = *indices.get(i)?;
    // returns the Option directly
    values.get(idx).copied()
}
```

</div>

The above Rust example is equivalent to the following, which does not use the
`?` operator. The version using `?` is more idiomatic.

```rust
fn access_value(
    indices: Vec<usize>,
    values: Vec<i32>,
    i: usize,
) -> Option<i32> {
    // matching through the & makes a copy of the i32
    let Some(&idx) = indices.get(i) else {
        return None;
    };
    // still returns the Option directly
    values.get(idx).copied()
}
```

The following example is also equivalent. It is not idiomatic (using `?` here is
more readable), but does demonstrate one of the helper methods.
`Option::and_then` is similar to [`std::optional::and_then` in
C++23](https://en.cppreference.com/w/cpp/utility/optional/and_then).

```rust
fn access_value(
    indices: Vec<usize>,
    values: Vec<i32>,
    i: usize,
) -> Option<i32> {
    // matching through the & makes a copy of the i32
    indices
        .get(i)
        .and_then(|idx| values.get(*idx))
        .copied()
}
```

These helper methods and others are described in detail in the documentation for
[`Option`](https://doc.rust-lang.org/std/option/enum.Option.html#implementations)
and
[`Result`](https://doc.rust-lang.org/std/result/enum.Result.html#implementations).

## Uncaught exceptions in `main`

In C++ when an exception is uncaught, it terminates the program with a non-zero
exit code and an error message. To achieve a similar result using `Result` in
Rust, `main` can be given a return type of `Result`.

<div class="comparison">

```cpp
#include <stdexcept>

int main() {
  throw std::runtime_error("oops");
}
```

```rust,ignore
fn main() -> Result<(), &'static str> {
    Err("oops")
}
```

</div>

The result type must be unit `()` and the error type can be any type that
implements the [`Debug`
trait](https://doc.rust-lang.org/std/fmt/trait.Debug.html).

```rust,no_run
#[derive(Debug)]
struct InterestingError {
    message: &'static str,
    other_interesting_value: i32,
}

fn main() -> Result<(), InterestingError> {
    Err(InterestingError {
        message: "oops",
        other_interesting_value: 9001,
    })
}
```

Running this program produces the output `Error: InterestingError { message:
"oops", other_interesting_value: 9001 }` with an exit code of `1`.

Result is not the only return type supported for `main`. See the [`Termination`
trait](https://doc.rust-lang.org/std/process/trait.Termination.html) for more
information.

## Limitations to forcing error handling with `Result`

Returning `Result` or `Option` does not give the usual benefits when used with
APIs that pass pre-allocated buffers by mutable reference. This is because the
buffer is accessible outside of the `Result` or `Option`, and so the compiler
cannot force handling of the error case.

For example, in the following example the result of `read_line` can be ignored,
resulting in logic errors in the program. However, since the buffer is required
to be initialized, it will not result in memory safety violations or undefined
behavior.

```rust
fn main() {
    let mut buffer = String::with_capacity(1024);
    std::io::stdin().read_line(&mut buffer);
    // use buffer
}
```

Rust will produce a warning in this case, because of the [`#[must_use]`
attribute](https://doc.rust-lang.org/reference/attributes/diagnostics.html#the-must_use-attribute)
on `Result`.

```text
warning: unused `Result` that must be used
 --> example.rs:3:5
  |
3 |     std::io::stdin().read_line(&mut buffer);
  |     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  |
  = note: this `Result` may be an `Err` variant, which should be handled
  = note: `#[warn(unused_must_use)]` on by default
help: use `let _ = ...` to ignore the resulting value
  |
3 |     let _ = std::io::stdin().read_line(&mut buffer);
  |     +++++++
```

`Option` does not have a `#[must_use]` attribute, so functions that return an
`Option` that must be handled (due to the `None` case indicating an error)
should be annotated with the `#[must_use]` attribute. For example, the `get`
method on slices returns `Option` and is [annotated as
`#[must_use]`](https://doc.rust-lang.org/src/core/slice/mod.rs.html#592-595).

## Designing and implementing error types

One challenge to handling errors in Rust compared to C++ is that because error
propagation in Rust is explicit, error values from different subsystems need to
be combined into a single type in order to be propagated further up the stack.
In C++, this requires no special effort.

The following example shows how such an error type is implemented manually.
Later examples show how the
[thiserror](https://docs.rs/thiserror/latest/thiserror/) and
[anyhow](https://docs.rs/anyhow/latest/anyhow/) crates can be used to reduce the
verbosity of the implementation.

<div class="comparison">

```cpp
#include <exception>

struct ErrorA : public std::exception {
  const char *msg = "ErrorA was produced";
  const char *what() const noexcept override {
    return msg;
  }
};

void mightThrowA() {}

struct ErrorB : public std::exception {
  const char *msg = "ErrorA was produced";
  const char *what() const noexcept override {
    return msg;
  }
};

void mightThrowB() {}

void process() {
  mightThrowA();
  mightThrowB();
}
```

```rust
use std::error::Error;
use std::fmt::Display;
use std::fmt::Formatter;

#[derive(Debug)]
struct ErrorA;

impl Display for ErrorA {
    fn fmt(
        &self,
        fmt: &mut Formatter<'_>,
    ) -> Result<(), std::fmt::Error> {
        write!(fmt, "ErrorA produced")
    }
}

impl Error for ErrorA {}

fn might_throw_A() -> Result<(), ErrorA> {
    Ok(())
}

#[derive(Debug)]
struct ErrorB;

impl Display for ErrorB {
    fn fmt(
        &self,
        fmt: &mut Formatter<'_>,
    ) -> Result<(), std::fmt::Error> {
        write!(fmt, "ErrorB produced")
    }
}

impl Error for ErrorB {}

fn might_throw_B() -> Result<(), ErrorB> {
    Ok(())
}

// This extra structure is needed to combine the errors
#[derive(Debug)]
enum ErrorAOrB {
    ErrorA(ErrorA),
    ErrorB(ErrorB),
}

impl Display for ErrorAOrB {
    fn fmt(
        &self,
        fmt: &mut Formatter<'_>,
    ) -> Result<(), std::fmt::Error> {
        match self {
            Self::ErrorA(err) => err.fmt(fmt),
            Self::ErrorB(err) => err.fmt(fmt),
        }
    }
}

impl Error for ErrorAOrB {}

impl From<ErrorA> for ErrorAOrB {
    fn from(err: ErrorA) -> Self {
        Self::ErrorA(err)
    }
}

impl From<ErrorB> for ErrorAOrB {
    fn from(err: ErrorB) -> Self {
        Self::ErrorB(err)
    }
}

fn process() -> Result<(), ErrorAOrB> {
    // the ? operator uses the From instance
    might_throw_A()?;
    might_throw_B()?;
    Ok(())
}
```

</div>

The following example uses the
[thiserror](https://docs.rs/thiserror/latest/thiserror/) crate to implement the
same thing as in the above example. The C++ version shown for comparison is the
same as in the previous example.

<div class="comparison">

```cpp
#include <exception>

struct ErrorA : public std::exception {
  const char *msg = "ErrorA was produced";
  const char *what() const noexcept override {
    return msg;
  }
};

void mightThrowA() {}

struct ErrorB : public std::exception {
  const char *msg = "ErrorA was produced";
  const char *what() const noexcept override {
    return msg;
  }
};

void mightThrowB() {}

void process() {
  mightThrowA();
  mightThrowB();
}
```

```rust,ignore,mdbook-runnable
use thiserror::Error;

#[derive(Debug, Error)]
#[error("ErrorA was produced")]
struct ErrorA;

fn might_throw_A() -> Result<(), ErrorA> {
    Ok(())
}

#[derive(Debug, Error)]
#[error("ErrorB was produced")]
struct ErrorB;

fn might_throw_B() -> Result<(), ErrorB> {
    Ok(())
}

#[derive(Debug, Error)]
enum ErrorAOrB {
    #[error("error from source A")]
    ErrorA(#[from] ErrorA),
    #[error("error from source B")]
    ErrorB(#[from] ErrorB),
}

fn process() -> Result<(), ErrorAOrB> {
    might_throw_A()?;
    might_throw_B()?;
    Ok(())
}
```

</div>

## Error types for applications

When implementing an application (as opposed to a library), it is often the case
that the specific type of error isn't as significant as the ability to easily
propagate them without the verbosity of the above example. For those cases, the
[anyhow](https://crates.io/crates/anyhow) crate provides mechanisms for
combining errors into a single error type, as well as the ability to produce
one-off errors. Since the errors types used in conjunction with anyhow still
need to implement the `std::error::Error` trait, anyhow is often used in
conjunction with thiserror.

Discriminating based on the type of the error, as one would do with `catch` in
C++, can be done with one of the [`downcast`
methods](https://docs.rs/anyhow/latest/anyhow/struct.Error.html#method.downcast).

<div class="comparison">

```cpp
#include <exception>

struct ErrorA : public std::exception {
  const char *msg = "ErrorA was produced";
  const char *what() const noexcept override {
    return msg;
  }
};

void mightThrowA() {}

struct ErrorB : public std::exception {
  const char *msg = "ErrorA was produced";
  const char *what() const noexcept override {
    return msg;
  }
};

void mightThrowB() {}

void process() {
  mightThrowA();
  mightThrowB();
}

int main() {
  try {
    process();
  } catch (ErrorA &err) {
    // handle ErrorA
  } catch (ErrorB &err) {
    // handle ErrorB
  }
}
```

```rust,ignore,mdbook-runnable
use thiserror::Error;

#[derive(Debug, Error)]
#[error("ErrorA was produced")]
struct ErrorA;

fn might_throw_A() -> Result<(), ErrorA> {
    Ok(())
}

#[derive(Debug, Error)]
#[error("ErrorB was produced")]
struct ErrorB;

fn might_throw_B() -> Result<(), ErrorB> {
    Ok(())
}

fn process() -> anyhow::Result<()> {
    might_throw_A()?;
    might_throw_B()?;
    Ok(())
}

fn main() {
    if let Err(err) = process() {
        if let Some(errA) =
            err.downcast_ref::<ErrorA>()
        {
            // handle ErrorA
        } else if let Some(errB) =
            err.downcast_ref::<ErrorB>()
        {
            // handle ErrorB
        }
    }
}
```

</div>

## Backtraces

Backtraces can be manually included with errors by defining a field with the
type [`Backtrace`](https://doc.rust-lang.org/std/backtrace/index.html). The
backtrace can be captured using the [`Backtrace::capture`
method](https://doc.rust-lang.org/std/backtrace/struct.Backtrace.html#method.capture).
The [module documentation](https://doc.rust-lang.org/std/backtrace/index.html)
describes the configuration required to enable backtraces.

Both [thiserror](https://docs.rs/thiserror/latest/thiserror/) and
[anyhow](https://docs.rs/anyhow/latest/anyhow/) have support for conveniently
adding backtrace information to errors. Instructions for including backtraces
are given on the main documentation page for each crate.
