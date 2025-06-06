# Setter and getter methods

Setters and getters work similarly in C++ and Rust, but are used less frequently
in Rust.

It would not be unusual to see the following representation of a two-dimensional
vector in C++, which hides its implementation and provides setters and getters
to access the fields. This choice would typically be made in case a
representation change (such as using polar instead of rectangular coordinates)
needed to be made later without breaking clients.

On the other hand, in Rust such a type would almost always be defined with
public fields.

<div class="comparison">

```cpp
class Vec2 {
  double x;
  double y;

public:
  Vec2(double x, double y) : x(x), y(y) {}
  double getX() { return x; }
  double getY() { return y; }

  // ... vector operations ...
};
```

```rust
pub struct Vec2 {
    // public fields instead of getters
    pub x: f64,
    pub y: f64,
}

impl Vec2 {
    // ... vector operations ...
}
```

</div>

One major reason for the difference is a limitation of the borrow checker. With
a getter function the entire structure is borrowed, preventing mutable use of
other fields of the structure.

The following program will not compile because `get_name()` borrows all of
`alice`.

```rust,ignore
struct Person {
    name: String,
    age: u32,
}

impl Person {
    fn get_name(&self) -> &String {
        &self.name
    }
}

fn main() {
    let mut alice = Person { name: "Alice".to_string(), age: 42 };
    let name = alice.get_name();

    alice.age = 43;

    println!("{}", name);
}
```

```text
error[E0506]: cannot assign to `alice.age` because it is borrowed
  --> example.rs:16:5
   |
14 |     let name = alice.get_name();
   |                ----- `alice.age` is borrowed here
15 |
16 |     alice.age = 43;
   |     ^^^^^^^^^^^^^^ `alice.age` is assigned to here but it was already borrowed
17 |
18 |     println!("{}", name);
   |                    ---- borrow later used here

error: aborting due to 1 previous error
```

Some additional reasons for the difference in approach are:

- Ergonomics: Public members make it possible to use pattern matching.
- Transparency of performance: A change in representation would dramatically
  change the costs involved with the getters. Exposing the representation makes
  the cost change visible.
- Control over mutability: Static lifetime checking of mutable references
  removes concerns of unintended mutation of the value through Rust's equivalent
  of observation pointers.

## Types with invariants and newtypes

When types need to preserve invariants but the benefits of exposing fields are
desired, a newtype pattern can be used. A wrapping "newtype" struct that
represents the data with an invariant is defined and access to the fields of the
underlying struct is provided by via a non-`mut` reference.

```rust
pub struct Vec2 {
    pub x: f64,
    pub y: f64,
}

/// Represents a 2-vector that has magnitude 1.
pub struct Normalized(Vec2); // note the private field

fn sqrt_approx_zero(x: f64) -> bool {
    x < 0.001
}

impl Normalized {
    pub fn from_vec2(v: Vec2) -> Option<Self> {
        if sqrt_approx_zero(v.x * v.x + v.y * v.x - 1.0) {
            Some(Self(v))
        } else {
            None
        }
    }

    // The getter provides a reference to the underlying Vec2 value
    // without permitting mutation.
    pub fn get(&self) -> &Vec2 {
        &self.0
    }
}
```


## Borrowing from indexed structures

A significant limitation that arises from the way that getter methods interact
with the borrow checker is that it isn't possible to mutably borrow multiple
elements from an indexed structure like a vector using a methods like
`Vec::get_mut`.

The built-in indexed types have several methods for creating split views onto a
structure. These can be used to create helper functions that match the
requirements of a specific application.

The Rustonomicon has [examples of implementing this
pattern](https://doc.rust-lang.org/nomicon/borrow-splitting.html), using both
safe and unsafe Rust.

## Setter methods

Setter methods also borrow the entire value, which causes the same problems as
getters that return mutable references. As with getter methods, setter methods
are mainly used when needed to preserve invariants.

{{#quiz setters_and_getters.toml}}
