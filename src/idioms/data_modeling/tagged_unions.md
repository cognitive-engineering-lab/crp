# Tagged unions and `std::variant`

## C-style tagged unions

Because unions cannot be used for type punning in C++, they are usually used
with a tag to discriminate between which variant of the union is active.

Rust's equivalent to union types are always tagged. They are a generalization of
Rust enums, where additional data may be associated with the enum variants.

<div class="comparison">

```cpp
enum Tag { Rectangle, Triangle };

struct Shape {
  Tag tag;
  union {
    struct {
      double width;
      double height;
    } rectangle;
    struct {
      double base;
      double height;
    } triangle;
  };

  double area() {
    switch (this->tag) {
    case Rectangle: {
      return this->rectangle.width *
             this->rectangle.height;
    }
    case Triangle: {
      return 0.5 * this->triangle.base *
             this->triangle.height;
    }
    }
  }
};
```

```rust
enum Shape {
    Rectangle { width: f64, height: f64 },
    Triangle { base: f64, height: f64 },
}

impl Shape {
    fn area(&self) -> f64 {
        match self {
            Shape::Rectangle {
                width,
                height,
            } => width * height,
            Shape::Triangle { base, height } => {
                0.5 * base * height
            }
        }
    }
}
```

</div>

When matching on an enum, Rust requires that all variants of the enum be
handled. In situations where `default` would be used with a C++ `switch` on the
tag, a wildcard can be used in the Rust `match`.

<div class="comparison">

```cpp
$#include <iostream>
$
$enum Tag { Rectangle, Triangle, Circle };
$
struct Shape {
$  Tag tag;
$  union {
$    struct {
$      double width;
$      double height;
$    } rectangle;
$    struct {
$      double base;
$      double height;
$    } triangle;
$    struct {
$      double radius;
$    } circle;
$  };
$
  void print_shape() {
    switch (this->tag) {
    case Rectangle: {
      std::cout << "Rectangle" << std::endl;
      break;
    }
    default: {
      std::cout << "Some other shape"
                << std::endl;
      break;
    }
    }
  }
};
```

```rust
# enum Shape {
#     Rectangle { width: f64, height: f64 },
#     Triangle { base: f64, height: f64 },
# }
#
impl Shape {
    fn print_shape(&self) {
        match self {
            Shape::Rectangle { .. } => {
                println!("Rectangle");
            }
            _ => {
                println!("Some other shape");
            }
        }
    }
}
```

</div>

Rust does not support C++-style fallthrough where some behavior can be done
before falling through to the next case. However, in Rust one can match on
multiple enum variants simultaneously, so long as the simultaneous match
patterns bind the same names with the same types.

```rust
# enum Shape {
#     Rectangle { width: f64, height: f64 },
#     Triangle { base: f64, height: f64 },
# }
#
impl Shape {
    fn bounding_area(&self) -> f64 {
        match self {
            Shape::Rectangle { height, width }
            | Shape::Triangle {
                height,
                base: width,
            } => width * height,
        }
    }
}
```

## Accessing the value without checking the discriminant

Unlike with C-style unions, Rust always requires matching on the discriminant
before accessing the values. If the variant is already known, e.g., due to an
earlier check, then the code can usually be refactored to encode the knowledge
in the type so that the second check (and corresponding error handling) can be
omitted.

A C++ program like the following requires more restructuring of the types to
achieve the same goal in Rust.

The corresponding Rust program requires defining separate types for each variant
of the `Shape` enum so that the fact that all of the value are of a given type
can be expressed in the type system by having an array of `Triangle` instead of
an array of `Shape`.

<div class="comparison">

```cpp
#include <ranges>
#include <vector>

// Uses the same Shape definition.
enum Tag { Rectangle, Triangle };

struct Shape {
  Tag tag;
  union {
    struct {
      double width;
      double height;
    } rectangle;
    struct {
      double base;
      double height;
    } triangle;
  };
};

std::vector<Shape> get_shapes() {
  return std::vector<Shape>{
      Shape{Triangle, {.triangle = {1.0, 1.0}}},
      Shape{Triangle, {.triangle = {1.0, 1.0}}},
      Shape{Rectangle, {.rectangle = {1.0, 1.0}}},
  };
}

std::vector<Shape> get_shapes();

int main() {
  std::vector<Shape> shapes = get_shapes();

  auto is_triangle = [](Shape shape) {
    return shape.tag == Triangle;
  };

  // Create an iterator that only sees the
  // triangles. (std::views::filter is from C++20,
  // but the same effect can be acheived with a
  // custom iterator.)
  auto triangles =
      shapes | std::views::filter(is_triangle);

  double total_base = 0.0;
  for (auto &triangle : triangles) {
    // Skip checking the tag because we know we
    // have only triangles.
    total_base += triangle.triangle.base;
  }

  return 0;
}
```

```rust
// Define a separate struct for each variant.
struct Rectangle { width: f64, height: f64 }
struct  Triangle { base: f64, height: f64 }

enum Shape {
    Rectangle(Rectangle),
    Triangle(Triangle),
}

fn get_shapes() -> Vec<Shape> {
    vec![
        Shape::Triangle(Triangle {
            base: 1.0,
            height: 1.0,
        }),
        Shape::Triangle(Triangle {
            base: 1.0,
            height: 1.0,
        }),
        Shape::Rectangle(Rectangle {
            width: 1.0,
            height: 1.0,
        }),
    ]
}

fn main() {
    let shapes = get_shapes();

    // This iterator only iterates over triangles
    // and demonstrates that by iterating over
    // the Triangle type instead of the Shape type.
    let triangles = shapes
        .iter()
        // Keep only the triangles
        .filter_map(|shape| match shape {
            Shape::Triangle(t) => Some(t),
            _ => None,
        });

    let mut total_base = 0.0;
    for triangle in triangles {
        // Because the iterator produces Triangles
        // instead of Shapes, base can be accessed
        // directly.
        total_base += triangle.base;
    }
}
```

</div>

This kind of use is common enough in Rust that the variants are often designed
to have their own types from the start.

This approach is also possible in C++. It is more commonly used along with
`std::variant` in C++17 or later.

## `std::variant` (since C++17)

When programming in C++ standards since C++17, `std::variant` can be used to
represent a tagged union in a way that has more in common with Rust enums.

```cpp
#include <variant>

struct Rectangle {
  double width;
  double height;
};

struct Triangle {
  double base;
  double height;
};

using Shape = std::variant<Rectangle, Triangle>;

double area(const Shape &shape) {
  return std::visit(
      [](auto &&arg) -> double {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Rectangle>) {
          return arg.width * arg.height;
        } else if constexpr (std::is_same_v<T, Triangle>) {
          return 0.5 * arg.base * arg.height;
        }
      },
      shape);
}
```

Because Rust doesn't depend on templates for this language feature, error
messages when a variant is missed or when a new variant is added are easier to
read, which removes one of the barriers to using tagged unions more frequently.
Compare the errors in C++ (using gcc) and Rust when the `Triangle` case is
omitted.

The following two programs have the same error: each fails to handle a case of
`Shape`.

<div class="comparison">

```cpp
#include <variant>

struct Rectangle {
  double width;
  double height;
};

struct Triangle {
  double base;
  double height;
};

using Shape = std::variant<Rectangle, Triangle>;

double area(const Shape &shape) {
  return std::visit(
      [](auto &&arg) -> double {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Rectangle>) {
          return arg.width * arg.height;
        }
      },
      shape);
}
```

```rust,ignore
enum Shape {
    Rectangle { width: f64, height: f64 },
    Triangle { base: f64, height: f64 },
}

impl Shape {
    fn area(&self) -> f64 {
        match self {
            Shape::Rectangle {
                width,
                height,
            } => width * height,
        }
    }
}
```

</div>

However, the error messages differ significantly.

<div class="comparison">

```text
example.cc: In instantiation of ‘area(const Shape&)::<lambda(auto:27&&)> [with auto:27 = const Triangle&]’:
/usr/include/c++/14.2.1/bits/invoke.h:61:36:   required from ‘constexpr _Res std::__invoke_impl(__invoke_other, _Fn&&, _Args&& ...) [with _Res = double; _Fn = area(const Shape&)::<lambda(auto:27&&)>; _Args = {const Triangle&}]’
   61 |     { return std::forward<_Fn>(__f)(std::forward<_Args>(__args)...); }
      |              ~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/include/c++/14.2.1/bits/invoke.h:96:40:   required from ‘constexpr typename std::__invoke_result<_Functor, _ArgTypes>::type std::__invoke(_Callable&&, _Args&& ...) [with _Callable = area(const Shape&)::<lambda(auto:27&&)>; _Args = {const Triangle&}; typename __invoke_result<_Functor, _ArgTypes>::type = double]’
   96 |       return std::__invoke_impl<__type>(__tag{}, std::forward<_Callable>(__fn),
      |              ~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   97 |                                         std::forward<_Args>(__args)...);
      |                                         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/include/c++/14.2.1/variant:1060:24:   required from ‘static constexpr decltype(auto) std::__detail::__variant::__gen_vtable_impl<std::__detail::__variant::_Multi_array<_Result_type (*)(_Visitor, _Variants ...)>, std::integer_sequence<long unsigned int, __indices ...> >::__visit_invoke(_Visitor&&, _Variants ...) [with _Result_type = std::__detail::__variant::__deduce_visit_result<double>; _Visitor = area(const Shape&)::<lambda(auto:27&&)>&&; _Variants = {const std::variant<Rectangle, Triangle>&}; long unsigned int ...__indices = {1}]’
 1060 |           return std::__invoke(std::forward<_Visitor>(__visitor),
      |                  ~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 1061 |               __element_by_index_or_cookie<__indices>(
      |               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 1062 |                 std::forward<_Variants>(__vars))...);
      |                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/include/c++/14.2.1/variant:1820:5:   required from ‘constexpr decltype(auto) std::__do_visit(_Visitor&&, _Variants&& ...) [with _Result_type = __detail::__variant::__deduce_visit_result<double>; _Visitor = area(const Shape&)::<lambda(auto:27&&)>; _Variants = {const variant<Rectangle, Triangle>&}]’
 1820 |                   _GLIBCXX_VISIT_CASE(1)
      |                   ^~~~~~~~~~~~~~~~~~~
/usr/include/c++/14.2.1/variant:1882:34:   required from ‘constexpr std::__detail::__variant::__visit_result_t<_Visitor, _Variants ...> std::visit(_Visitor&&, _Variants&& ...) [with _Visitor = area(const Shape&)::<lambda(auto:27&&)>; _Variants = {const variant<Rectangle, Triangle>&}; __detail::__variant::__visit_result_t<_Visitor, _Variants ...> = double]’
 1882 |             return std::__do_visit<_Tag>(
      |                    ~~~~~~~~~~~~~~~~~~~~~^
 1883 |               std::forward<_Visitor>(__visitor),
      |               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 1884 |               static_cast<_Vp>(__variants)...);
      |               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
example.cc:17:20:   required from here
   17 |   return std::visit(
      |          ~~~~~~~~~~^
   18 |       [](auto &&arg) -> double {
      |       ~~~~~~~~~~~~~~~~~~~~~~~~~~
   19 |         using T = std::decay_t<decltype(arg)>;
      |         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   20 |         if constexpr (std::is_same_v<T, Rectangle>) {
      |         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   21 |           return arg.width * arg.height;
      |           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   22 |         }
      |         ~
   23 |       },
      |       ~~
   24 |       shape);
      |       ~~~~~~
example.cc:23:7: error: no return statement in ‘constexpr’ function returning non-void
   23 |       },
      |       ^
example.cc: In lambda function:
example.cc:23:7: warning: control reaches end of non-void function [-Wreturn-type]
```


```text
error[E0004]: non-exhaustive patterns: `&Shape::Triangle { .. }` not covered
 --> example.rs:8:15
  |
8 |         match self {
  |               ^^^^ pattern `&Shape::Triangle { .. }` not covered
  |
note: `Shape` defined here
 --> example.rs:1:6
  |
1 | enum Shape {
  |      ^^^^^
2 |     Rectangle { width: f64, height: f64 },
3 |     Triangle { base: f64, height: f64 },
  |     -------- not covered
  = note: the matched value is of type `&Shape`
help: ensure that all possible cases are being handled by adding a match arm with a wildcard pattern or an explicit pattern as shown
  |
12~             } => width * height,
13~             &Shape::Triangle { .. } => todo!(),
  |
```

</div>

## Using unsafe Rust to avoid checking the discriminant

In situations where rewriting code to use the [above
approach](#accessing-the-value-without-checking-the-discriminant) is not
possible, one can check the discriminant anyway and then use the [`unreachable!`
macro](https://doc.rust-lang.org/std/macro.unreachable.html) to avoid handling
the impossible case. However, that still involves actually checking the
discriminant. If the cost of checking the discriminant must be avoided, then the
[unsafe function
`unreachable_unchecked`](https://doc.rust-lang.org/std/hint/fn.unreachable_unchecked.html)
can be used to both avoid handling the case and to indicate to the compiler that
the optimizer should assume that the case cannot be reached, so the discriminant
check can be optimized away.

Much like how in the C++ example accessing an inactive variant is undefined
behavior, reaching `unreachable_unchecked` is also undefined behavior.

```rust
# enum Shape {
#     Rectangle { width: f64, height: f64 },
#     Triangle { base: f64, height: f64 },
# }
#
# impl Shape {
#     fn area(&self) -> f64 {
#         match self {
#             Shape::Rectangle {
#                 width,
#                 height,
#             } => width * height,
#             Shape::Triangle { base, height } => {
#                 0.5 * base * height
#             }
#         }
#     }
# }
#
# fn get_triangles() -> Vec<Shape> {
#     vec![
#         Shape::Triangle {
#             base: 1.0,
#             height: 1.0,
#         },
#         Shape::Triangle {
#             base: 1.0,
#             height: 1.0,
#         },
#     ]
# }
#
use std::hint::unreachable_unchecked;

fn main() {
    let mut total_base = 0.0;
    for triangle in get_triangles() {
        match triangle {
            Shape::Triangle { base, .. } => {
                total_base += base;
            }
            _ =>
            // SAFETY: get_triangles is guaranteed to produce triangles, so
            // other cases aren't reachable.
            unsafe {
                unreachable_unchecked();
            },
        }
    }
}
```

{{#quiz tagged_unions.toml}}
