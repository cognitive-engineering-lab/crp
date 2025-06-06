# Constructors

In C++, constructors initialize objects. At the point when a constructor is executed, storage for the object has been
allocated and the constructor is only performing initialization.

Rust does not have constructors in the same way as C++. In Rust, there is a
single fundamental way to create an object, which is to initialize all of its
members at once. The term "constructor" or "constructor method" in Rust refers
to something more like a factory: a static method associated with a type (i.e.,
a method that does not have a `self` parameter), which returns a value of the
type.

<div class="comparison">

```cpp
$#include <thread>
$unsigned int cpu_count() {
$    return std::thread::hardware_concurrency();
$}
$
class ThreadPool {
  unsigned int num_threads;

public:
  ThreadPool() : num_threads(cpu_count()) {}
  ThreadPool(unsigned int nt) : num_threads(nt) {}
};

int main() {
  ThreadPool p1;
  ThreadPool p2(4);
}
```

```rust
# fn cpu_count() -> usize {
#     std::thread::available_parallelism().unwrap().get()
# }
#
struct ThreadPool {
  num_threads: usize
}

impl ThreadPool {
    fn new() -> Self {
        Self { num_threads: cpu_count() }
    }

    fn with_threads(nt: usize) -> Self {
        Self { num_threads: nt }
    }
}

fn main() {
    let p1 = ThreadPool::new();
    let p2 = ThreadPool::with_threads(4);
}
```

</div>


In Rust, typically the primary constructor for a type is named `new`, especially if it
takes no arguments. (See the chapter on [default
constructors](./constructors/default_constructors.html).) Constructors based on
some specific property of the value are usually named `with_<something>`, e.g.,
`ThreadPool::with_threads`. See the [naming
guidelines](https://rust-lang.github.io/api-guidelines/naming.html) for the
conventions on how to name constructor methods in Rust.

If the fields to be initialized are visible, there is a reasonable default
value, and the value does not manage a resource, then it is also common to use
record update syntax to initialize a value based on some default value.

```rust
struct Point {
    x: i32,
    y: i32,
    z: i32,
}

impl Point {
    const fn zero() -> Self {
        Self { x: 0, y: 0, z: 0 }
    }
}

fn main() {
    let x_unit = Point {
        x: 1,
        ..Point::zero()
    };
}
```

Despite the name, "record update syntax" does not modify a record but instead
creates a new value based on another one, taking ownership of it in order to do
so.

## Storage allocation vs initialization

In Rust, the actual construction of a structure or enum value occurs where the
structure construction syntax (e.g., `ThreadPool { ... }`) is, after the
evaluation of the expressions for the fields (e.g., `cpu_count()`).

A significant implication of this difference is that storage is not allocated
for a struct in Rust at the point where the constructor method (such as
`ThreadPool::with_threads`) is called, and in fact is not allocated until after the
values of the fields of a struct have been computed (in terms of the semantics
of the language &mdash; the optimizer may still avoid the copy). Therefore there is no
straightforward way in Rust to translate patterns such as a class which stores a pointer to
itself upon construction (in Rust, this requires tools like [`Pin`](https://doc.rust-lang.org/std/pin/struct.Pin.html) and [`MaybeUninit`](https://doc.rust-lang.org/std/mem/union.MaybeUninit.html)).

## Fallible constructors

In C++, the primary way constructors can indicate failure is by throwing
exceptions. In Rust, because constructors are normal static methods, fallible
constructors can instead return `Result` (akin to `std::expected`) or `Option`
(akin to `std::optional`).[^NonZero]

[^NonZero]: An alternative approach here would be to use `NonZero<usize>` as the
    type so that the error case wasn't possible in the first place.

<div class="comparison">

```cpp
#include <iostream>
#include <stdexcept>

class ThreadPool {
  unsigned int num_threads;

public:
  ThreadPool(unsigned int nt) : num_threads(nt) {
    if (num_threads == 0) {
      throw std::domain_error(
          "Cannot have zero threads");
    }
  }
};

int main() {
  try {
    ThreadPool p(0);
    // use p here
  } catch (const std::domain_error &e) {
    std::cout << e.what() << std::endl;
  }
}
```

```rust
struct ThreadPool {
    num_threads: usize,
}

#[derive(Debug)]
enum ThreadPoolError {
    ZeroThreads,
}

impl ThreadPool {
    fn with_threads(
        nt: usize,
    ) -> Result<Self, ThreadPoolError> {
        if nt == 0 {
            Err(ThreadPoolError::ZeroThreads)
        } else {
            Ok(Self { num_threads: nt })
        }
    }
}

fn main() {
    match ThreadPool::with_threads(0) {
        Err(err) => println!("{:?}", err),
        Ok(p) => {
            // use p here
        }
    }
}
```

</div>

See [the chapter on exceptions](./exceptions.md) for more information on
how C++ exceptions and exception handling translate to Rust.
