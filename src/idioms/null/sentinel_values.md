# Sentinel values

Sentinel values are in-band value that indicates a special situation, such as
having reached the end of valid data in an iterator.

## `nullptr`

Many designs in C++ borrow the convention from C of using a null pointer as a
sentinel value for a method that returns owned pointers. For example, a method
that parses a large structure may produce `nullptr` in the case of failure.

A similar situation in Rust would make use of the type
`Option<Box<LargeStructure>>`.

<div class="comparison">

```cpp
#include <memory>

class LargeStructure {
  int field;
  // many fields ...
};

std::unique_ptr<LargeStructure>
parse(char *data, size_t len) {
  // ...

  // on failure
  return nullptr;
}
```

```rust
struct LargeStructure {
    field: i32,
    // many fields ...
}

fn parse(
    data: &[u8],
) -> Option<Box<LargeStructure>> {
    // ...

    // on failure
    None
}
```

</div>

The `Box<T>` type has the same meaning as `std::unique_ptr<T>` in terms of being
a uniquely owned pointer to some `T` on the heap, but unlike `std::unique_ptr`,
it cannot be null. Rust's `Option<T>` (which is similar to `std::optional<T>` in
C++) can represent optional pointers when used in conjunction with `Box<T>`, as
in `Optional<Box<T>>`. In [those cases (and in some other
cases)](../data_modeling/template_specialization.md#niche-optimization) the
compiler optimizes the representation to be the same size as `Box<T>` by
leveraging the fact that `Box` cannot be null.

In Rust it is also common to pay the cost for the extra byte to use a return
type of `Result<T, E>` (which is akin to `std::expected` in C++23) in order to
make the reason for the failure available at runtime.

## Integer sentinels

When a possibly-failing function produces an integer, it is also common to use
an otherwise unused or unlikely integer value as a sentinel value, such as `0`
or `INT_MAX`.

In Rust, the `Option` type is used for this purpose. In cases where the zero
value really is not possible to produce, as with the GCD algorithm below, the
type `NonZero<T>` can be used to indicate that fact. As with `Option<Box<T>>`,
the compiler optimizes the representation to make use of the unused value (in
this case `0`) to represent the `None` case to ensure that the representation of
`Option<NonZero<T>>` is the same as the representation of `Option<T>`.

<div class="comparison">

```cpp
#include <algorithm>

int gcd(int a, int b) {
  if (b == 0 || a == 0) {
    // returns 0 to indicate invalid input
    return 0;
  }

  while (b != 0) {
    int temp = b;
    b = a % b;
    a = temp;
  }
  return std::abs(a);
}
```

```rust
use std::num::NonZero;

fn gcd(
    mut a: i32,
    mut b: i32,
) -> Option<NonZero<i32>> {
    if a == 0 || b == 0 {
        return None;
    }

    while b != 0 {
        let temp = b;
        b = a % b;
        a = temp;
    }
    // At this point, a is guaranteed to not be
    // zero. The `Some` case from `NonZero::new`
    // has a different meaning than the `Some`
    // returned from this function, but here it
    // happens to coincide.
    NonZero::new(a.abs())
}
#
# fn main() {
#     assert!(gcd(5, 0) == None);
#     assert!(gcd(0, 5) == None);
#     assert!(gcd(5, 1) == NonZero::new(1));
#     assert!(gcd(1, 5) == NonZero::new(1));
#     assert!(gcd(2 * 2 * 3 * 5 * 7, 2 * 2 * 7 * 11) == NonZero::new(2 * 2 * 7));
#     assert!(gcd(2 * 2 * 7 * 11, 2 * 2 * 3 * 5 * 7) == NonZero::new(2 * 2 * 7));
# }
```

</div>

As an aside, it is also possible to avoid the redundant check for zero at the end, and
without using unsafe Rust, by preserving the non-zeroness property throughout
the algorithm.

```rust
use std::num::NonZero;

fn gcd(x: i32, mut b: i32) -> Option<NonZero<i32>> {
    if b == 0 {
        return None;
    }

    // a is guaranteed to be non-zero, so we record the fact in the type of a.
    let mut a = NonZero::new(x)?;

    while let Some(temp) = NonZero::new(b) {
        b = a.get() % b;
        a = temp;
    }
    Some(a.abs())
}
#
# fn main() {
#     assert!(gcd(5, 0) == None);
#     assert!(gcd(0, 5) == None);
#     assert!(gcd(5, 1) == NonZero::new(1));
#     assert!(gcd(1, 5) == NonZero::new(1));
#     assert!(gcd(2 * 2 * 3 * 5 * 7, 2 * 2 * 7 * 11) == NonZero::new(2 * 2 * 7));
#     assert!(gcd(2 * 2 * 7 * 11, 2 * 2 * 3 * 5 * 7) == NonZero::new(2 * 2 * 7));
# }
```

## `std::optional`

In situations where `std::optional` would be used as a sentinel value in C++,
`Option` can be used for the same purpose in Rust. The main difference between
the two is that safe Rust requires either explicitly checking whether the value is
`None`, while in C++ one can attempt to access the value without checking (at
the risk of undefined behavior).

{{#quiz sentinel_values.toml}}
