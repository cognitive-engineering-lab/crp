# Type promotions and conversions

## lvalue to rvalue

In C++ lvalues are automatically converted to rvalues when needed.

In Rust the equivalent of lvalues are "place expressions" (expressions that
represent memory locations) and the equivalent of rvalues are "value
expressions". Place expressions are automatically converted to value expressions
when needed.

<div class="comparison">

```cpp
int main() {
  // Local variables are lvalues,
  int x(0);
  // and therefore may be assigned to.
  x = 42;

  // x is converted to an rvalue when needed.
  int y = x + 1;
}
```

```rust
fn main() {
    // Local variables are place expressions,
    let mut x = 0;
    // and therefore may be assigned to.
    x = 42;

    // x is converted to a value expression when
    // needed.
    let y = x + 1;
}
```

</div>

## Array to pointer

In C++, arrays are automatically converted to pointers as required.

The equivalent to this in Rust is the automatic conversion of vector and array
references to slice references.

<div class="comparison">

```cpp
#include <cstring>

int main() {
  char example[6] = "hello";
  char other[6];

  // strncpy takes arguments of type char*
  strncpy(other, example, 6);
}
```

```rust
fn third(ts: &[char]) -> Option<&char> {
    ts.get(2)
}

fn main() {
    let vec: Vec<char> = vec!['a', 'b', 'c'];
    let arr: [char; 3] = ['a', 'b', 'c'];

    third(&vec);
    third(&arr);
}
```

</div>

Because slice references can be easily used in a memory-safe way, it is
generally recommended in Rust to define functions in terms of slice references
instead of in terms of references to vectors or arrays, unless vector-specific
or array-specific functionality is needed.

Unlike in C++ where the conversion from arrays to pointers is built into the
language, this is actually a general mechanism provided by the [`Deref`
trait](https://doc.rust-lang.org/std/ops/trait.Deref.html), which provides one
kind of [user-defined conversion](./user-defined_conversions.md).

## Function to pointer

In C++ functions and static member functions are automatically converted to
function pointers.

Rust performs the same conversion. In addition to functions and members that do
not take `self` as an argument, constructors (proper constructors) also have
function type and can be converted to function pointers. Non-capturing closures
do not have function type, but can also be converted to function pointers.

<div class="comparison">

```cpp
int twice(int n) {
  return n * n;
}

struct MyPair {
  int x;
  int y;

  MyPair(int x, int y) : x(x), y(y) {}

  static MyPair make() {
    return MyPair{0, 0};
  }
};

int main() {
  // convert a function to a function pointer
  int (*twicePtr)(int) = twice;
  int result = twicePtr(5);

  // Per C++23 11.4.5.1.6, can't take the address
  // of a constructor.
  // MyPair (*ctor)(int, int) = MyPair::MyPair;
  // MyPair pair = ctor(10, 20);

  // convert a static method to a function
  // pointer
  MyPair (*methodPtr)() = MyPair::make;
  MyPair pair2 = methodPtr();

  // convert a non-capturing closure to a
  // function pointer
  int (*closure)(int) = [](int x) -> int {
    return x * 5;
  };
  int closureRes = closure(2);
}
```

```rust
fn twice(x: i32) -> i32 {
    x * x
}

struct MyPair(i32, i32);

impl MyPair {
    fn new() -> MyPair {
        MyPair(0, 0)
    }
}

fn main() {
    // convert a function to a function pointer
    let twicePtr: fn(i32) -> i32 = twice;
    let res = twicePtr(5);

    // convert a constructor to a function pointer
    let ctorPtr: fn(i32, i32) -> MyPair = MyPair;
    let pair = ctorPtr(10, 20);

    // convert a static method to a function
    // pointer
    let methodPtr: fn() -> MyPair = MyPair::new;
    let pair2 = methodPtr();

    // convert a non-capturing closure to a
    // function pointer
    let closure: fn(i32) -> i32 = |x: i32| x * 5;
    let closureRes = closure(2);
}
```

</div>

## Numeric promotion and numeric conversion

In C++ there are several kinds of implicit conversions that occur between
numeric types. The most commonly encountered are numeric promotions, which
convert numeric types to larger types.

These lossless conversions are not implicit in Rust. Instead, they must be
performed explicitly using the `Into::into()` method. These conversions are
provided by implementations of the
[`From`](https://doc.rust-lang.org/std/convert/trait.From.html) and
[`Into`](https://doc.rust-lang.org/std/convert/trait.Into.html) traits. The list
of conversions provided by the Rust standard library is [listed on the
documentation
page](https://doc.rust-lang.org/std/convert/trait.From.html#implementors) for
the trait.

<div class="comparison">

```cpp
int main() {
  int x(42);
  long y = x;

  float a(1.0);
  double b = a;
}
```

```rust
fn main() {
    let x: i32 = 42;
    let y: i64 = x.into();

    let a: f32 = 1.0;
    let b: f64 = a.into();
}
```

</div>

There are several implicit conversions that occur in C++ that are not lossless.
For example, integers can be implicitly converted to unsigned integers in C++.

In Rust, these conversions are also required to be explicit and are provided by
the [`TryFrom`](https://doc.rust-lang.org/std/convert/trait.TryFrom.html) and
[`TryInto`](https://doc.rust-lang.org/std/convert/trait.TryInto.html) traits
which require handling the cases where the value does not map to the other type.

<div class="comparison">

```cpp
int main() {
  int x(42);
  unsigned int y(x);

  float a(1.0);
  double b(a);
}
```

```rust
use std::convert::TryInto;

fn main() {
    let x: i32 = 42;
    let y: u32 = match x.try_into() {
        Ok(x) => x,
        Err(err) => {
            panic!("Can't convert! {:?}", err);
        }
    };
}
```

</div>

Some conversions that occur in C++ are supported by neither `From` nor `TryFrom`
because there is not a clear choice of conversion or because they are not
value-preserving. For example, in C++ `int32_t` can implicitly be converted to
`float` despite `float` not being able to represent all 32 bit integers
precisely, but in Rust there is no `TryFrom<i32>` implementation for `f32`.

In Rust the only way to convert from an `i32` to an `f32` is with the [`as`
operator](https://doc.rust-lang.org/stable/reference/expressions/operator-expr.html#r-expr.as.coercions).
The operator can actually be used to convert between other primitive types as
well and does not panic or produce undefined behavior, but may not convert in
the desired way (e.g., it may use a different rounding mode than desired or it
may truncate rather than saturate as desired).

<div class="comparison">

```cpp
#include <cstdint>

int main() {
  int32_t x(42);
  float a = x;
}
```

```rust
fn main() {
    let x: i32 = 42;
    let a: f32 = x as f32;
}
```

</div>

### `isize` and `usize`

In the Rust standard library the `isize` and `usize` types are used for values
intended to used be indices (much like `size_t` in C++). However, their use for
other purposes is usually discouraged in favor of using explicitly sized types
such as `u32`. This results a situation where values of type `u32` have to be
converted to `usize` for use in indexing, but `Into<usize>` is not implemented
for `u32`.

In these cases, best practice is to use `TryInto`, and if further error handling
of the failure cause is not desired, to call `unwrap`, creating a panic at the
point of conversion.

This is preferred because it prevents the possibility of moving forward with an
incorrect value. E.g., consider converting a `u64` to a `usize` that has a
32-bit representation with `as`, which truncates the result. A value that is one
greater than the `u32::MAX` will truncate to `0`, which would probably result in
successfully retrieving the wrong value from a data structure, thus masking a
bug and producing unexpected behavior.

### Enums

In C++ enums can be implicitly converted to integer types.

In Rust the conversion requires the use of the `as` operator, and providing
`From` and `TryFrom` implementations to move back and forth between the enum and
its representation type is recommended. Examples and additional details are
given in the [chapter on enums](./data_modeling/enums.md).

## Qualification conversion

In C++ qualification conversions enable the use of const (or volatile) values
where the const (or volatile) qualifier is not expected.

In Rust the equivalent enables the use of `mut` variables and `mut` references
to be used where non-`mut` variables or references are expected.

<div class="comparison">

```cpp
#include <iostream>
#include <string>

void display(const std::string &msg) {
  std::cout << "Displaying: " << msg << std::endl;
}

int main() {
  // no const qualifier
  std::string message("hello world");

  // used where const expected
  display(message);
}
```

```rust
fn display(msg: &str) {
    println!("{}", msg);
}

fn main() {
    let mut s: String = "hello world".to_string();
    let message: &mut str = s.as_mut();
    display(message);
}
```

</div>

## Integer literals

In C++ integer literals with no suffix indicating type have the smallest type in
which they can fit from `int`, `long int`, or `long long int`. When the literal
is then assigned to a variable of a different type, an implicit conversion is
performed.

In Rust, integer literals have their type inferred depending on context. When
there is insufficient information to infer a type either `i32` is assumed or may
require some type annotation to be given.

<div class="comparison">

```cpp
#include <cstdint>
#include <iostream>

int main() {
  // Compiles without error (but with a warning).
  uint32_t x = 4294967296;

  // assumes int
  auto y = 1;

  // literal is given a larger type, so it prints
  // correctly
  std::cout << 4294967296 << std::endl;

  // these work as expected
  std::cout << INT64_C(4294967296) << std::endl;

  uint64_t z = INT64_C(4294967296);
  std::cout << z << std::endl;
}
```

```rust
fn main() {
    // error: literal out of range for `u32`
    // let x: u32 = 4294967296;

    // assumes i32
    let y = 1;

    // fails to compile because it is inferred as i32
    // print!("{}", 4294967296);

    // These work, though.
    println!("{}", 4294967296u64);

    let z: u64 = 4294967296;
    println!("{}", z);
}
```

</div>

## Safe bools

The safe bool idiom exists to make it possible to use types as conditions. Since
C++11 this idiom is straightforward to implement.

In Rust instead of converting the value to a boolean, the normal idiom matches
on the value instead. Depending on the situation, the mechanism used for
matching might be `match`, `if let`, or `let else`.

<div class="comparison">

```cpp
struct Wire {
  bool ready;
  unsigned int value;

  explicit operator bool() const { return ready; }
};

int main() {
  Wire w{false, 0};
  // ...

  if (w) {
    // use w.value
  } else {
    // do something else
  }
}
```

```rust
enum Wire {
    Ready(u32),
    NotReady,
}

fn main() {
    let wire = Wire::NotReady;
    // ...

    // match
    match wire {
        Wire::Ready(v) => {
            // use value v
        }
        Wire::NotReady => {
            // do something else
        }
    }

    // if let
    if let Wire::Ready(v) = wire {
        // use value v
    }

    // let else
    let Wire::Ready(v) = wire else {
        // do something that doesn't continue,
        // like early return
        return;
    };
}
```

</div>

## User-defined conversions

User-defined conversions are covered in a [separate
chapter](./user-defined_conversions.md).

{{#quiz promotions_and_conversions.toml}}
