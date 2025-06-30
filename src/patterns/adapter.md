# Adapter pattern

In C++, if an existing class needs to implement a new interface, the adapter
pattern is normally used. The pattern involves defining a wrapper class that
implements the interface by delegating to the methods on the original class.

In Rust, the same pattern is possible and is [sometimes necessary due to the
orphan
rule](https://doc.rust-lang.org/book/ch20-02-advanced-traits.html#using-the-newtype-pattern-to-implement-external-traits-on-external-types).
However, because traits can be implemented where *either* the type or the trait
is defined, usually it is possible to just implement the trait for the type
directly, without the need for a wrapper.

The following example adds an interface to `std::string` in C++ and `String` in
Rust for use with a template function defined in the library. When dynamic
dispatch is needed, there is no change to how to implement the trait for the
existing type in Rust.

<div class="comparison">

```cpp
#include <concepts>
#include <iostream>
#include <string>

template <typename T>
concept doubleable = requires(const T t) {
  { t.twice() } -> std::same_as<T>;
};

template <doubleable T>
T quadruple(const T &x) {
  return x.twice().twice();
}

struct DoubleableString {
  std::string str;

  DoubleableString twice() const {
    return DoubleableString{this->str +
                            this->str};
  }
};

int main() {
  auto s = quadruple(
      DoubleableString{std::string("a")});
  std::cout << s.str << std::endl;
}
```

```rust
trait Doubleable {
    fn twice(&self) -> Self;
}

impl Doubleable for String {
    fn twice(&self) -> Self {
        self.clone() + self
    }
}

fn quadruple<T: Doubleable>(x: T) -> T {
    x.twice().twice()
}

fn main() {
    let s = quadruple(String::from("a"));
    println!("{}", s);
}
```

</div>

## Extension traits

The usual approach to adding functionality to an existing type in C++ is to
define the additional functionality as functions.

Rust can similarly add functionality by defining freestanding functions. Rust
also supports the ability to add methods to existing types. It does so by using
the same mechanism as described in the previous section. By using a blanket
implementation, methods can even be added to any type that implements some other
trait. This is the approach used by the [`itertools`
crate](https://docs.rs/itertools/latest/itertools/) to add additional
functionality to anything that implements the `Iterator` trait.

```rust
trait Middle {
    type Output;
    fn middle(&mut self) -> Option<Self::Output>;
}

impl<T: ExactSizeIterator> Middle for T {
    type Output = T::Item;

    fn middle(&mut self) -> Option<Self::Output> {
        let len = self.len();
        if len > 0 && len % 2 == 1 {
            self.nth(len / 2)
        } else {
            None
        }
    }
}

fn main() {
    println!("{:?}", [1, 2, 3].iter().middle());
    println!("{:?}", [1, 2, 3].iter().map(|n| n + 1).middle());
}
```

The `map` method returns a different type than `iter`, but `middle` can be
called on the result of either one.

{{#quiz adapter.toml}}
