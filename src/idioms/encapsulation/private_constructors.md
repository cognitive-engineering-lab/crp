# Private constructors

In C++ constructors for classes can be made private by declaring them private,
or by defining a class using `class` and using the default private visibility.

In Rust, constructors (the actual constructors, not ["constructor
methods"](../constructors.md)) for structs are visible from wherever the
type and all fields are visible. To achieve similar visibility restrictions as
in the C++ example, an additional private field needs to be added to the struct
in Rust. Because Rust supports zero-sized types, the additional field can have
no performance cost. The [unit
type](https://doc.rust-lang.org/std/primitive.unit.html) has zero size and can
be used for this purpose.

<div class="comparison">

```cpp
#include <string>

struct Person {
  std::string name;
  int age;

private:
  Person() = default;
};

int main() {
  // fails to compile, Person::Person() private
  // Person nobody;

  // fails to compile since C++20
  // Person alice{"Alice", 42};
  return 0;
}
```

```rust
mod person {
    pub struct Person {
        pub name: String,
        pub age: i32,
        _private: (),
    }

    impl Person {
        pub fn new(
            name: String,
            age: i32,
        ) -> Person {
            Person {
                name,
                age,
                _private: (),
            }
        }
    }
}

use person::*;

fn main() {
    // field `_private` of struct `person::Person`
    // is private
    // let alice = Person {
    //     name: "Alice".to_string(),
    //     age: 42,
    //     _private: (),
    // };

    // cannot construct `person::Person` with
    // struct literal syntax due to private fields
    // let bob = Person {
    //     name: "Bob".to_string(),
    //     age: 55,
    // };

    let carol =
        Person::new("Carol".to_string(), 20);
    // Can match on the public fields, and then
    // use .. to ignore the remaning ones.
    let Person { name, age, .. } = carol;
}
```

</div>

## Enums

Unlike C++ unions, but like `std::variant`, Rust enums do not have direct
control over the visibility of their variants or the fields of their variants.
In the following example, the `circle` variant of the `Shape` union is not
public, so it can only be accessed from within the definition of `Shape`, as it
is by the `make_circle` static method.

```cpp
#include <iostream>

struct Triangle {
  double base;
  double height;
};

struct Circle {
  double radius;
};

union Shape {
  Triangle triangle;

private:
  Circle circle;

public:
  static Shape make_circle(double radius) {
    Shape s;
    s.circle = Circle(radius);
    return s;
  };
};

int main() {
  Shape triangle;
  triangle.triangle = Triangle{1.0, 2.0};
  Shape circle = Shape::make_circle(1.0);

  // fails to compile
  // circle.circle = Circle{1.0};

  // fails to compile
  // std::cout << shape.circle.radius;
}
```

In Rust visibility modifiers cannot be applied to individual enum variants or
their fields.

```rust
mod shape {
    pub enum Shape {
        Triangle { base: f64, height: f64 },
        Circle { radius: f64 },
    }
}

use shape::*;

fn main() {
    // Variant constructor is accesssible despite not being marked pub.
    let triangle = Shape::Triangle {
        base: 1.0,
        height: 2.0,
    };

    let circle = Shape::Circle { radius: 1.0 };

    // Fields accessbile despite not being marked pub.
    match circle {
        Shape::Triangle { base, height } => {
            println!("Triangle: {}, {}", base, height);
        }
        Shape::Circle { radius } => {
            println!("Circle {}", radius);
        }
    }
}
```

Instead, to control construction of and pattern matching on the enum
implementation, one of two approaches can be taken. The first controls
construction of and access to the fields, but not inspection of which variant is
active.

```rust
mod shape {
    pub struct Triangle {
        pub base: f64,
        pub height: f64,
        _private: (),
    }
    pub struct Circle {
        pub radius: f64,
        _private: (),
    }

    pub enum Shape {
        Triangle(Triangle),
        Circle(Circle),
    }

    impl Shape {
        pub fn new_triangle(base: f64, height: f64) -> Shape {
            Shape::Triangle(Triangle {
                base,
                height,
                _private: (),
            })
        }

        pub fn new_circle(radius: f64) -> Shape {
            Shape::Circle(Circle {
                radius,
                _private: (),
            })
        }
    }
}

use shape::*;

fn main() {
    let triangle = Shape::new_triangle(1.0, 2.0);
    let circle = Shape::new_circle(1.0);

    match circle {
        Shape::Triangle(Triangle { base, height, .. }) => {
            println!("Triangle: {}, {}", base, height);
        }
        Shape::Circle(Circle { radius, .. }) => {
            println!("Circle: {}", radius);
        }
    }
}
```

The second places the enum in a struct with a private field, preventing both
construction and inspection from outside of the module.

```rust
mod shape {
    enum ShapeKind {
        Triangle { base: f64, height: f64 },
        Circle { radius: f64 },
    }

    pub struct Shape(ShapeKind);

    impl Shape {
        pub fn new_circle(radius: f64) -> Shape {
            Shape(ShapeKind::Circle { radius })
        }

        pub fn new_triangle(base: f64, height: f64) -> Shape {
            Shape(ShapeKind::Triangle { base, height })
        }

        pub fn print(&self) {
            match self.0 {
                ShapeKind::Triangle { base, height } => {
                    println!("Triangle: {}, {}", base, height);
                }
                ShapeKind::Circle { radius } => {
                    println!("Circle: {}", radius);
                }
            }
        }
    }
}

use shape::*;

fn main() {
    let triangle = Shape::new_triangle(1.0, 2.0);
    let circle = Shape::new_circle(1.0);

    // Does not compile because Shape has private fields.
    // match circle {
    //   Shape(_) -> {}
    // }

    circle.print();
}
```

If the purpose of making the variants private is to ensure that invariants are
met, then it can be useful to expose the implementing enum (`ShapeKind`) but not
the field of the wrapping struct (`Shape`), with the invariants only being
guaranteed when the wrapping struct is used. In this case, it is necessary to
make the field private and define a getter function, since otherwise the field
would be modifiable, possibly violating the invariant that the wrapping struct
represents.

```rust
mod shape {
    pub enum ShapeKind {
        Triangle { base: f64, height: f64 },
        Circle { radius: f64 },
    }

    // The field of Shape is private.
    pub struct Shape(ShapeKind);

    impl Shape {
        pub fn new(kind: ShapeKind) -> Option<Shape> {
            // ... check invariants ...
            Some(Shape(kind))
        }

        pub fn get_kind(&self) -> &ShapeKind {
            &self.0
        }
    }
}

use shape::*;

fn main() {
    let triangle = Shape::new(ShapeKind::Triangle {
        base: 1.0,
        height: 2.0,
    });
    let Some(circle) = Shape::new(ShapeKind::Circle { radius: 1.0 }) else {
        return;
    };

    // Does not compile because Shape has private fields.
    // match circle {
    //   Shape(c) => {}
    // };

    match circle.get_kind() {
        ShapeKind::Triangle { base, height } => {
            println!("Triangle: {}, {}", base, height);
        }
        ShapeKind::Circle { radius } => {
            println!("Circle: {}", radius);
        }
    }
}
```

The situation in Rust resembles the situation in C++ when using `std::variant`,
for which it is not possible to make the variants themselves private. Instead
either the constructors for the types that form the variants can be made private
or the variant can be wrapped in a class with appropriate visibility controls.

## Rust's `#[non_exhaustive]` annotation

If a struct or enum is intended to be public within a
[crate](https://doc.rust-lang.org/book/ch07-01-packages-and-crates.html), but
should not be constructed outside of the crate, then the `#[non_exhaustive]`
attribute can be used to constrain construction. The attribute can be applied to
both structs and to individual enum variants with the same effect as adding a
private field.

However, the attribute applies the constraint at the level of the crate, not at
the level of a module.

```rust
#[non_exhaustive]
pub struct Person {
    pub name: String,
    pub age: i32,
}

pub enum Shape {
    #[non_exhaustive]
    Triangle { base: f64, height: f64 },
    #[non_exhaustive]
    Circle { radius: f64 },
}
```

The attribute is more typically used to force clients of a library to include
the wildcard when matching on the struct fields, making it so that adding
additional fields to a struct is not breaking change (i.e., that it does not
[require the increase of the major version component when using semantic
versioning](https://doc.rust-lang.org/cargo/reference/semver.html)).

Applying the `#[non_exhaustive]` attribute to the enum itself makes it as if one
of the variants were private, requiring a wildcard when matching on the variant
itself. This has the same effect in terms of versioning as when used on a struct
but is less advantageous. In most cases, code failing to compile when a new enum
variant is added is desirable, since that indicates a new case that requires
handling logic.

{{#quiz private_constructors.toml}}
