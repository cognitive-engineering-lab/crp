# Template specialization

Template specialization in C++ makes it possible for a template entity to have
different implementations for different parameters. Most STL implementations
make use of this to, for example, provide a [space-efficient representation of
`std::vector<bool>`](https://en.cppreference.com/w/cpp/container/vector_bool).

Because of the possibility of template specialization, when a C++ function
operates on values of a template class like `std::vector`, the function is
essentially defined in terms of the interface provided by the template class,
rather than for a specific implementation.

To accomplish the same thing in Rust requires defining the function in terms of
a trait for the interface against which it operates. This enables clients to
select their choice of representation for data by using any concrete type that
implements the interface.

This is more practical to do in Rust than in C++, because generics not being a
general metaprogramming facility means that [generic entities can be type
checked
locally](./templates.md#a-note-on-type-checking-and-type-errors),
making them easier to define. It is more common to do in Rust than in C++
because Rust does not have [implementation
inheritance](./inheritance_and_reuse.md), so there is a
sharper line between interface and implementation than there is in C++.

The following example shows how a Rust function can be implemented so that
different concrete representations can be selected by a client. For a compact
bit vector representation, the example uses the
[`BitVec`](https://docs.rs/bitvec/latest/bitvec/vec/struct.BitVec.html) type
from the [bitvec crate](https://docs.rs/bitvec/latest/bitvec/). `BitVec` is
intended intended to provide an API similar to `Vec<bool>` or
`std::vector<bool>`.

<div class="comparison">

```cpp
#include <string>
#include <vector>

template <typename T>
void push_if_even(int n,
                  std::vector<T> &collection,
                  T item) {
  if (n % 2 == 0) {
    collection.push_back(std::move(item));
  }
}

int main() {
  // Operate on the default std::vector
  // implementation
  std::vector<std::string> v{"a", "b"};
  push_if_even(2, v, std::string("c"));

  // Operate on the (likely space-optimized)
  // std::vector implementation
  std::vector<bool> bv{false, true};
  push_if_even(2, bv, false);
}
```

```rust,ignore
// The Extend trait is for types that support
// appending values to the collection.
fn push_if_even<T, I: Extend<T>>(
    n: u32,
    collection: &mut I,
    item: T,
) {
    if n % 2 == 0 {
        collection.extend([item]);
    }
}

use bitvec::prelude::*;

fn main() {
    // Operate on Vec
    let mut v =
        vec!["a".to_string(), "b".to_string()];
    push_if_even(2, &mut v, "c".to_string());

    // Operate on BitVec
    let mut bv = bitvec![0, 1];
    push_if_even(2, &mut bv, 0);
}
```

</div>

## Trade-offs between generics and templates

Because generic functions can only interact with generic values in ways defined
by the trait bounds, it is easier to test generic implementations. In
particular, code testing a generic implementation only has to consider the
possible behaviors of the given trait.

For a comparison, consider the following programs.

<div class="comparison">

```cpp
template <totally_ordered T>
T max(const T &x, const T &y) {
  return (x > y) ? x : y;
}

template <>
int max(const int &x, const int &y) {
  return (x > y) ? x + 1 : y + 1;
}
```

```rust
fn max<'a, T: Ord>(x: &'a T, y: &'a T) -> &'a T {
    if x > y {
        x
    } else {
        y
    }
}
```

</div>

In the Rust program, _parametricity_ means that (assuming safe Rust) from the
type alone one can tell that if the function returns, it must return exactly one
of `x` or `y`. This is because the trait bound `Ord` doesn't give any way to
construct new values of type `T`, and the use of references doesn't give any way
for the function to store one of `x` or `y` from an earlier call to return in a
later call.

In the C++ program, a call to `max` with `int` as the template parameter will
give a distinctly different result than with any other parameter because of the
template specialization enabling the behavior of the function to vary based on
the type.

The trade-off is that in Rust specialized implementations are harder to use
because they must have different names, but that they are easier to write
because it is easier to write generic code while being confident about its
correctness.

## Niche optimization

There are several cases where the Rust compiler will perform optimizations to
achieve more efficient representations. Those situations are all ones where the
efficiency gains do not otherwise change the observable behavior of the code.

[The most common case is with the `Option`
type](https://doc.rust-lang.org/std/option/index.html#representation). When
`Option` is used with a type where the compiler can tell that there are unused
values, one of those unused values will be used to represent the `None` case, so
that `Option<T>` will not require an extra word of memory to indicate the
discriminant of the enum.

This optimization is applied to reference types (`&` and `&mut`), since
references cannot be null. It is also applied to `NonNull<T>`, which represents
a non-null pointer to a value of type `T`, and to `NonZeroU8` and other non-zero
integral types. The optimization for the reference case is what makes
`Option<&T>` and `Option<&mut T>` safer equivalents to using non-owning
observation pointers in C++.

{{#quiz template_specialization.toml}}
