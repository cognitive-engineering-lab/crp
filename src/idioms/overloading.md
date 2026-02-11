# Overloading

C++ supports overloading of functions, so long as the invocations of the
functions can be distinguished by the number or types of their arguments.

Rust does not support this kind of function overloading. Instead, Rust has a few
different mechanisms (some of which C++ also has) for achieving the effects of
overloading in a way that interacts better with type inference. The mechanisms
usually involve making the commonalities between the overloaded functions
apparent in the code.

<div class="comparison">

```cpp
#include <string>

double twice(double x) {
  return x + x;
}

int twice(int x) {
  return x + x;
}
```

```rust
fn twice(x: f64) -> f64 {
    x + x
}

// error[E0428]: the name `twice` is defined multiple times
// fn twice(x: i32) -> i32 {
//     x + x
// }
```

</div>

In practice, an example like the above would also likely be implemented in a
more structured way even in C++, using templates.

When phrased this way, the example can be translated to Rust, with the notable
addition of [requiring a trait bound on the
type](./data_modeling/concepts.md).

<div class="comparison">

```cpp
template <typename T>
T twice(T x) {
  return x + x;
}
```

```rust
fn twice<T>(x: T) -> T::Output
where
    T: std::ops::Add<T>,
    T: Copy,
{
    x + x
}
```

</div>

## Overloaded methods

In C++ it is possible to have methods with the same name but different
signatures on the same type. In Rust there can be at most one method with the
same name for each trait implementation and at most one inherent method with the
same name for a type.

In cases where there are multiple methods with the same names because the method
is defined for multiple traits, the desired method must be distinguished at the
call site by specifying the trait.

```rust
trait TraitA {
    fn go(&self) -> String;
}

trait TraitB {
    fn go(&self) -> String;
}

struct MyStruct;

impl MyStruct {
    fn go(&self) -> String {
        "Called inherent method".to_string()
    }
}

impl TraitA for MyStruct {
    fn go(&self) -> String {
        "Called Trait A method".to_string()
    }
}

impl TraitB for MyStruct {
    fn go(&self) -> String {
        "Called Trait B method".to_string()
    }
}

fn main() {
    let my_struct = MyStruct;

    // Calling the inherent method
    println!("{}", my_struct.go());

    // Calling the method from TraitA
    println!("{}", TraitA::go(&my_struct));

    // Calling the method from TraitB
    println!("{}", TraitB::go(&my_struct));
}
```

One exception to this is when the methods are all from the same generic trait
with different type parameters for the implementations. In that case, if
the signature is sufficient to determine which implementation to use, the trait
does not need to be specified to resolve the method. This is common when using
the [`From` trait](https://doc.rust-lang.org/std/convert/trait.From.html).

```rust
struct Widget;

impl From<i32> for Widget {
    fn from(x: i32) -> Widget {
        Widget
    }
}

impl From<f32> for Widget {
    fn from(x: f32) -> Widget {
        Widget
    }
}

fn main() {
    // Calls <Widget as From<i32>>::from
    let w1 = Widget::from(5);
    // Calls <Widget as From<f32>>::from
    let w2 = Widget::from(1.0);
}
```

## Overloaded operators

In C++ most operators can be overloaded either with a free-standing
function or by providing a method defining the operator on a class.

Rust provides operators via implementation of specific traits. Implementing a
method of the same name as required by the trait will not make a type usable
with the operator if the trait is not implemented.

<div class="comparison">

```cpp
struct Vec2 {
  double x;
  double y;

  Vec2 operator+(const Vec2 &other) const {
    return Vec2{x + other.x, y + other.y};
  }
};

int main() {
  Vec2 a{1.0, 2.0};
  Vec2 b{3.0, 4.0};
  Vec2 c = a + b;
}
```

```rust
#[derive(Clone, Copy)]
struct Vec2 {
    x: f64,
    y: f64,
}

impl std::ops::Add for &Vec2 {
    type Output = Vec2;

    // Note that the type of self here is &Vec2.
    fn add(self, other: Self) -> Vec2 {
        Vec2 {
            x: self.x + other.x,
            y: self.y + other.y,
        }
    }
}

fn main() {
    let a = Vec2 { x: 1.0, y: 2.0 };
    let b = Vec2 { x: 3.0, y: 4.0 };
    let c = &a + &b;
}
```

</div>

Additionally, sometimes it is best to provide trait implementations for various
combinations of reference types, especially for types that implement the [`Copy
trait`](./constructors/copy_and_move_constructors.md), since they are
likely to want to be used either with or without taking a reference. For the
example above, that involve defining four implementations.

```rust
#[derive(Clone, Copy)]
struct Vec2 {
    x: f64,
    y: f64,
}

impl std::ops::Add<&Vec2> for &Vec2 {
    type Output = Vec2;

    fn add(self, other: &Vec2) -> Vec2 {
        Vec2 {
            x: self.x + other.x,
            y: self.y + other.y,
        }
    }
}

// If Vec2 weren't so small, it might be desirable to re-use space in the below
// implementations, since they take ownership.

impl std::ops::Add<Vec2> for &Vec2 {
    type Output = Vec2;

    fn add(self, other: Vec2) -> Vec2 {
        Vec2 {
            x: self.x + other.x,
            y: self.y + other.y,
        }
    }
}

impl std::ops::Add<&Vec2> for Vec2 {
    type Output = Vec2;

    fn add(self, other: &Vec2) -> Vec2 {
        Vec2 {
            x: self.x + other.x,
            y: self.y + other.y,
        }
    }
}

impl std::ops::Add<Vec2> for Vec2 {
    type Output = Vec2;

    fn add(self, other: Vec2) -> Vec2 {
        Vec2 {
            x: self.x + other.x,
            y: self.y + other.y,
        }
    }
}

fn main() {
    let a = Vec2 { x: 1.0, y: 2.0 };
    let b = Vec2 { x: 3.0, y: 4.0 };
    let c = a + b;
}
```

The repetition can be addressed by defining a macro.

```rust
#[derive(Clone, Copy)]
struct Vec2 {
    x: f64,
    y: f64,
}

macro_rules! impl_add_vec2 {
    ($lhs:ty, $rhs:ty) => {
        impl std::ops::Add<$rhs> for $lhs {
            type Output = Vec2;

            fn add(self, other: $rhs) -> Vec2 {
                Vec2 {
                    x: self.x + other.x,
                    y: self.y + other.y,
                }
            }
        }
    };
}

impl_add_vec2!(&Vec2, &Vec2);
impl_add_vec2!(&Vec2, Vec2);
impl_add_vec2!(Vec2, &Vec2);
impl_add_vec2!(Vec2, Vec2);

fn main() {
    let a = Vec2 { x: 1.0, y: 2.0 };
    let b = Vec2 { x: 3.0, y: 4.0 };
    let c = a + b;
}
```

## Default arguments

Default arguments in C++ are sometimes implemented in terms of function
overloading.

Rust does not have default arguments. Instead, arguments with `Option` type can
be used to provide a similar effect.

<div class="comparison">

```cpp
unsigned int shift(unsigned int x,
                   unsigned int shiftAmount) {
  return x << shiftAmount;
}

unsigned int shift(unsigned int x) {
  return shift(x, 2);
}

int main() {
  unsigned int a = shift(7); // shifts by 2
}
```

```rust
use std::ops::Shl;

fn shift(
    x: u32,
    shift_amount: Option<u32>,
) -> u32 {
    let a = shift_amount.unwrap_or(2);
    x.shl(a)
}

fn main() {
    let res = shift(7, None); // shifts by 2
}
```

</div>

## Unrelated overloads

The lack of completely ad hoc overloading in Rust encourages the definition of
traits that capture essential commonalities between types, so that functions can
be implemented in terms of those interfaces and used generally. However, it also
sometime encourages the anti-pattern of defining of traits that only capture
incidental commonalities (such as having methods of the same name).

It is better programming practice in those cases to simply define separate
functions, rather than to shoehorn in a trait where no real commonality exists.

This is commonly seen in Rust in the naming conventions for constructor static
methods. Instead of them all being named `new` with different arguments, they
are [usually given names of the form
`from_something`](https://rust-lang.github.io/api-guidelines/naming.html), where
the `something` varies based on from what the value is being constructed, or a
more specific name if appropriate.

```rust
struct Vec3 {
    x: f64,
    y: f64,
    z: f64,
}

impl Vec3 {
    fn from_x(x: f64) -> Vec3 {
        Vec3 { x, y: 0.0, z: 0.0 }
    }

    fn from_y(y: f64) -> Vec3 {
        Vec3 { x: 0.0, y, z: 0.0 }
    }

    fn diagonal(d: f64) -> Vec3 {
        Vec3 { x: d, y: d, z: d }
    }
}
```

This differs from the conversion methods supported by the `From` and `Into`
traits, which have the additional purpose of supporting trait bounds on generic
functions which should take any type convertible to a specific type.

{{#quiz overloading.toml}}
