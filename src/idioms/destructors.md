# Destructors and resource cleanup

In C++, a destructor for a class `T` is defined by providing a special member
function `~T()`. To achieve the equivalent in Rust, the [`Drop`
trait](https://doc.rust-lang.org/std/ops/trait.Drop.html) is implemented for a
type.

For an example, see [the chapter on copy and move
constructors](./constructors/copy_and_move_constructors.md#user-defined-constructors).

`Drop` implementations play the same role as destructors in C++ for types that
manage resources. That is, they enable cleanup of resources owned by the value
at the end of the value's lifetime.

In Rust the `Drop::drop` method of a value is called automatically by a
destructor when the variable that owns the value goes out of scope. Unlike in
C++, the drop method cannot be called manually. Instead the automatic "drop
glue" implicitly calls the destructors of fields.

## Lifetimes, destructors, and destruction order

C++ destructors are called in reverse order of construction when variables go
out of scope, or for dynamically allocated objects, when they are deleted. This
includes destructors of moved-from objects.

In Rust, the drop order for items going out of scope is similar to that of C++
(reverse order of declaration). If additional specific details about the drop
order are needed (e.g., for writing unsafe code), the full rules for the drop
order are described in [the language
reference](https://doc.rust-lang.org/reference/destructors.html). However,
moving an object in Rust does not leave a moved-from object on which a
destructor will be called.

<div class="comparison">

```cpp
#include <iostream>
#include <utility>

struct A {
  int id;

  A(int id) : id(id) {}

  // copy constructor
  A(A &other) : id(other.id) {}

  // move constructor
  A(A &&other) : id(other.id) {
    other.id = 0;
  }

  // destructor
  ~A() {
    std::cout << id << std::endl;
  }
};

int accept(A x) {
  return x.id;
} // the destructor of x is called after the
  // return expression is evaluated

// Prints:
// 2
// 3
// 0
// 1
int main() {
  A x(1);
  A y(2);

  accept(std::move(y));

  A z(3);

  return 0;
}
```

```rust
struct A {
    id: i32,
}

impl Drop for A {
    fn drop(&mut self) {
        println!("{}", self.id)
    }
}

fn accept(x: A) -> i32 {
    return x.id;
}

// Prints:
// 2
// 3
// 1
fn main() {
    let x = A { id: 1 };
    let y = A { id: 2 };

    accept(y);

    let z = A { id: 3 };
}
```

</div>

In Rust, after ownership of `y` is moved into the function `accept`, there is
no additional object remaining, and so there is no additional `Drop::drop` call
(which in the C++ example prints `0`).

Rust's drop methods do run when leaving scope due to a panic, though not if the
panic occurs in a destructor that was called in response to an initial panic.

The drop order of fields in Rust is essentially the reverse of that of
non-static class members in C++. Again, the specific details of what happens in
a Rust destructor are given in [the language
reference](https://doc.rust-lang.org/reference/destructors.html#r-destructors.operation).

<div class="comparison">

```cpp
#include <iostream>
#include <string>

struct Part {
  std::string name;

  ~Part() {
    std::cout << "Dropped " << name << std::endl;
  }
};

struct Widget {
  Part part1;
  Part part2;
  Part part3;
};

int main() {
  Widget w{"1", "2", "3"};
  // Prints:
  // 3
  // 2
  // 1
}
```

```rust
struct Part(&'static str);

impl Drop for Part {
    fn drop(&mut self) {
        println!("{}", self.0);
    }
}

struct Widget {
    part1: Part,
    part2: Part,
    part3: Part,
}

fn main() {
    let w = Widget {
        part1: Part("1"),
        part2: Part("2"),
        part3: Part("3"),
    };
    // Prints:
    // 1
    // 2
    // 3
}
```

</div>

## Early cleanup and explicitly destroying values

In C++ you can explicitly destroy an object. This is mainly useful for
situations where placement new has been used to allocate the object at a
specific memory location, and so the destructor will not be implicitly called.

However, once the destructor has been explicitly called, [it may not be called
again, even implicitly](https://eel.is/c++draft/class.dtor#note-8). Thus the
destructor can't be used for early cleanup. Instead, either the class must be
designed with a separate cleanup method that releases the resources but leaves
the object in a state where the destructor can be called or the function using
the object must be structured so that the variable goes out of scope at the
desired time.

In Rust, values can be dropped early for early cleanup by using
[`std::mem::drop`](https://doc.rust-lang.org/std/mem/fn.drop.html). This works
because ([for non-`Copy`
types](./constructors/copy_and_move_constructors.md#trivially-copyable-types))
ownership of the object is actually transferred to `std::mem::drop` function,
and so `Drop::drop` is called at the end of `std::mem::drop` when the lifetime
of the parameter ends.

Thus, `std::mem::drop` can be used for early cleanup of resources without having
to restructure a function to force variables out of scope early.

For example, the following allocates a large vector on the heap, but explicitly
drops it before allocating a second large vector on the heap, reducing the
overall memory usage.

```rust
fn main() {
    let v = vec![0u32; 100000];
    // ... use v

    std::mem::drop(v);
    // can no longer use v here

    let v2 = vec![0u32; 100000];
    // ... use v2
}
```

{{#quiz destructors.toml}}
