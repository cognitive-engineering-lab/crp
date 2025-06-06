# Abstract classes, interfaces, and dynamic dispatch

In C++ when an interface will be used with dynamic dispatch to resolve invoked
methods, the interface is defined using an abstract class. Types that implement
the interface inherit from the abstract class. In Rust the interface is given by
a *trait*, which is then implemented for the types that support that trait.
Programs can then be written over *trait objects* that use that trait as their
base type.

The following example defines an interface, two implementations of that
interface, and a function that takes an argument that satisfies the interface.
In C++ the interface is defined with an abstract class with pure virtual
methods, and in Rust the interface is defined with a trait. In both languages,
the function (`printArea` in C++ and `print_area` in Rust) invokes a method
using dynamic dispatch.

<div class="comparison">

```cpp
#include <iostream>
#include <memory>

// Define an abstract class for an interface
struct Shape {
  Shape() = default;
  virtual ~Shape() = default;
  virtual double area() = 0;
};

// Implement the interface for a concrete class
struct Triangle : public Shape {
  double base;
  double height;

  Triangle(double base, double height)
      : base(base), height(height) {}

  double area() override {
    return 0.5 * base * height;
  }
};

// Implement the interface for a concrete class
struct Rectangle : public Shape {
  double width;
  double height;

  Rectangle(double width, double height)
      : width(width), height(height) {}

  double area() override {
    return width * height;
  }
};

// Use an object via a reference to the interface
void printArea(Shape &shape) {
  std::cout << shape.area() << std::endl;
}

int main() {
  Triangle triangle = Triangle{1.0, 1.0};

  printArea(triangle);

  // Use an object via an owned pointer to the
  // interface
  std::unique_ptr<Shape> shape;
  if (true) {
    shape = std::make_unique<Rectangle>(1.0, 1.0);
  } else {
    shape = std::make_unique<Triangle>(
        std::move(triangle));
  }

  // Convert to a reference to the interface
  printArea(*shape);
}
```

```rust
// Define an interface
trait Shape {
    fn area(&self) -> f64;
}

struct Triangle {
    base: f64,
    height: f64,
}

// Implement the interface for a concrete type
impl Shape for Triangle {
    fn area(&self) -> f64 {
        0.5 * self.base * self.height
    }
}

struct Rectangle {
    width: f64,
    height: f64,
}

// Implement the interface for a concrete type
impl Shape for Rectangle {
    fn area(&self) -> f64 {
        self.width * self.height
    }
}

// Use a value via a reference to the interface
fn print_area(shape: &dyn Shape) {
    println!("{}", shape.area());
}

fn main() {
    let triangle = Triangle {
        base: 1.0,
        height: 1.0,
    };

    print_area(&triangle);

    // Use a value via an owned pointer to the
    // interface
    let shape: Box<dyn Shape> = if true {
        Box::new(Rectangle {
            width: 1.0,
            height: 1.0,
        })
    } else {
        Box::new(triangle)
    };

    // Convert to a reference to the interface
    print_area(shape.as_ref());
}
```

</div>

There are several places where the Rust implementation differs slightly from the
C++ implementation.

In Rust, a trait's methods are always visible whenever the trait itself is
visible. Additionally, the fact that a type implements a trait is always visible
whenever both the trait and the type are visible. These properties of Rust
explain the lack of visibility declarations in places where one might find them
in C++.

In C++, to associate methods with a type rather than value of that type, you use
the `static` keyword. In Rust, non-static methods take an explicit `self` parameter.
This syntactic choice makes it possible to indicate (in way similar to other parameters) whether the
method mutates the object (by taking `&mut self` instead of `&self`) and whether
it takes ownership of the object (by taking `self` instead of `&self`).

Rust methods do not need to be declared as virtual. Because of differences in
vtable representation, all methods for a type are available for dynamic
dispatch. Types of values that use vtables are indicated with the `dyn` keyword.
This is further described [below](#vtables-and-rust-trait-object-types).

Additionally, Rust does not have an equivalent for the virtual destructor
declaration because in Rust every vtable includes the drop behavior (whether
given by a user defined `Drop` implementation or not) required for the value.

## Vtables and Rust trait object types

C++ and Rust both requires some kind of indirection to perform dynamic dispatch
against an interface. In C++ this indirection takes the form of a pointer to the
abstract class (instead of the derived concrete class), making use of a vtable
to resolve the virtual method.

In the above Rust example, the type `dyn Shape` is the type of a trait object
for the `Shape` trait. A trait object includes a vtable along with the
underlying value.

In C++ all objects whose class inherits from a class with a virtual method have
a vtable in their representation, whether dynamic dispatch is used or not.
Pointers or references to objects are the same size as pointers to objects
without virtual methods, but every object includes its vtable.

In Rust, vtables are present only when values are represented as trait objects.
The reference to the trait object is twice the size of a normal reference since
it includes both the pointer to the value and the pointer to the vtable. In the
Rust example above, the local variable `triangle` in `main` does not have a
vtable in its representation, but when the reference to it is converted to a
reference to a trait object (so that it can be passed to `print_area`), that
does include a pointer to the vtable.

Additionally, just as abstract classes in C++ cannot be used as the type of a
local variable, the type of a parameter of a function, or the type of a return
value of a function, trait object types in Rust cannot be used in corresponding
contexts. In Rust, this is enforced by the type `dyn Shape` not implementing the
`Sized` marker trait, preventing it from being used in contexts that require
knowing the size of a type statically.

The following example shows some places where a trait object type can and cannot
be used due to not implementing `Sized`. The uses forbidden in Rust would also
be forbidden in C++ because `Shape` is an abstract class.

```rust
# trait Shape {
#     fn area(&self) -> f64;
# }
#
# struct Triangle {
#     base: f64,
#     height: f64,
# }
#
# impl Shape for Triangle {
#     fn area(&self) -> f64 {
#         0.5 * self.base * self.height
#     }
# }
#
fn main() {
    // Local variables must have a known size.
    // let v: dyn Shape = Triangle { base: 1.0, height: 1.0 };

    // References always have a known size.
    let shape: &dyn Shape = &Triangle {
        base: 1.0,
        height: 1.0,
    };
    // Boxes also always have a known size.
    let boxed_shape: Box<dyn Shape> = Box::new(Triangle {
        base: 1.0,
        height: 1.0,
    });

    // Types like Option<T> include the value of type T directly, and so also
    // need to know the size of T.
    // let v: Option<dyn Shape> = Some(Triangle { base: 1.0, height: 1.0 });
}

// Parameter types must have a known size.
// fn print_area(shape: dyn Shape) { }
fn print_area(shape: &dyn Shape) {}
```

The decision to include the vtable in the reference instead of in the value is
one part of what makes it reasonable to use traits both for polymorphism via
dynamic dispatch and for [polymorphism via static dispatch, where one would use
concepts in C++](./concepts.md).

## Limitations of trait objects in Rust

In Rust, not all traits can be used as the base trait for trait objects. The
most commonly encountered restriction is that traits that require knowledge of
the object's size via a `Sized` supertrait are not `dyn`-compatible. There are
[additional
restrictions](https://doc.rust-lang.org/reference/items/traits.html#dyn-compatibility).

## Trait objects and lifetimes

Objects which are used with dynamic dispatch may contain pointers or references
to other objects. In C++ the lifetimes of those references must be tracked
manually by the programmer.

Rust checks the bounds on the lifetimes of references that the trait objects may
contain. If the bounds are not given explicitly, they are determined according
to the [lifetime elision
rules](https://doc.rust-lang.org/reference/lifetime-elision.html#r-lifetime-elision.trait-object).
The bound is part of the type of the trait object.

Usually the elision rules pick the correct lifetime bound. Sometimes, the rules
result in surprising error messages from the compiler. In those situations or
when the compiler cannot determine which lifetime bound to assign, the bound may
be given manually. The following example shows explicitly what the inferred
lifetimes are for a structure storing a trait object and for the `print_area`
function.

```rust
# trait Shape {
#     fn area(&self) -> f64;
# }
#
# struct Triangle {
#     base: f64,
#     height: f64,
# }
#
# impl Shape for Triangle {
#     fn area(&self) -> f64 {
#         0.5 * self.base * self.height
#     }
# }
#
struct Scaled {
    scale: f64,
    // 'static is the lifetime that would be inferred by the lifetime elision
    // rule [lifetime-elision.trait-object.default].
    shape: Box<dyn Shape + 'static>,
}

impl Shape for Scaled {
    fn area(&self) -> f64 {
        self.scale * self.shape.area()
    }
}

// These are the lifetimes that would be inferred by the lifetime elision rule
// [lifetime-elision.function.implicit-lifetime-parameters] for the reference
// and [lifetime-elision.trait-object.containing-type-unique] for the trait
// bound.
fn print_area<'a>(shape: &'a (dyn Shape + 'a)) {
    println!("{}", shape.area());
}

fn main() {
    let triangle = Triangle {
        base: 1.0,
        height: 1.0,
    };
    print_area(&triangle);

    let scaled_triangle = Scaled {
        scale: 2.0,
        shape: Box::new(triangle),
    };
    print_area(&scaled_triangle);
}
```

{{#quiz abstract_classes.toml}}
