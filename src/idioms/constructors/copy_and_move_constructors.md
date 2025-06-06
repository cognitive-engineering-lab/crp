# Copy and move constructors

In both C++ and Rust, one rarely has to write copy or move constructors (or
their Rust equivalents) by hand. In C++ this is because the implicit definitions
are good enough for most purposes, especially when using smart pointers (i.e.,
following [the rule of
zero](https://en.cppreference.com/w/cpp/language/rule_of_three)). In Rust this
is because move semantics are the default, and the automatically derived
implementations of the `Clone` and `Copy` traits are good enough for most
purposes.

For the following C++ classes, the implicitly defined copy and move constructors
are sufficient. The equivalent in Rust uses a derive macro provided by the
standard library to implement the corresponding traits.

<div class="comparison">

```cpp
$#include <memory>
$#include <string>
$
struct Age {
  unsigned int years;

  Age(unsigned int years) : years(years) {}

  // copy and move constructors and destructor
  // implicitly declared and defined
};

struct Person {
  Age age;
  std::string name;
  std::shared_ptr<Person> best_friend;

  Person(Age age,
         std::string name,
         std::shared_ptr<Person> best_friend)
      : age(std::move(age)), name(std::move(name)),
        best_friend(std::move(best_friend)) {}

  // copy and move constructors and destructor
  // implicitly declared and defined
};
```

```rust
use std::rc::Rc;

#[derive(Clone, Copy)]
struct Age {
    years: u32,
}

#[derive(Clone)]
struct Person {
    age: Age,
    name: String,
    best_friend: Rc<Person>,
}
```

One point to note about the previous example is that `std::shared_ptr` and `Rc`
differ slightly in terms of thread safety. See the chapter on [type
equivalents](../type_equivalents.html#pointers) for more details.

</div>

## User-defined constructors

On the other hand, the following example requires a user-defined copy and move
constructor because it manages a resource (a pointer acquired from a C library).
The equivalent in Rust requires a custom implementation of the `Clone` trait.[^deleter]

[^deleter]: Another common approach to the C++ version of the example is to use
    the `Deleter` template argument for `std::unique_ptr`. The version shown in
    the example was chosen to make the correspondence to Rust version clearer.

<div class="comparison">

```cpp
#include <cstdlib>
#include <cstring>

// widget.h
struct widget_t;
widget_t *alloc_widget();
void free_widget(widget_t *);
void copy_widget(widget_t *dst, widget_t *src);

// widget.cc
class Widget {
  widget_t *widget;

public:
  Widget() : widget(alloc_widget()) {}

  Widget(const Widget &other) : widget(alloc_widget()) {
    copy_widget(widget, other.widget);
  }

  Widget(Widget &&other) : widget(other.widget) {
    other.widget = nullptr;
  }

  ~Widget() {
    free_widget(widget);
  }
};
```

```rust
# mod example {
mod widget_ffi {
    // Models an opaque type.
    // See https://doc.rust-lang.org/nomicon/ffi.html#representing-opaque-structs
    #[repr(C)]
    pub struct CWidget {
        _data: [u8; 0],
        _marker: core::marker::PhantomData<(
            *mut u8,
            core::marker::PhantomPinned,
        )>,
    }

    extern "C" {
        pub fn make_widget() -> *mut CWidget;
        pub fn copy_widget(
            dst: *mut CWidget,
            src: *mut CWidget,
        );
        pub fn free_widget(ptr: *mut CWidget);
    }
}

use self::widget_ffi::*;

struct Widget {
    widget: *mut CWidget,
}

impl Widget {
    fn new() -> Self {
        Widget {
            widget: unsafe { make_widget() },
        }
    }
}

impl Clone for Widget {
    fn clone(&self) -> Self {
        let widget = unsafe { make_widget() };
        unsafe {
            copy_widget(widget, self.widget);
        }
        Widget { widget }
    }
}

impl Drop for Widget {
    fn drop(&mut self) {
        unsafe { free_widget(self.widget) };
    }
}
# }
```

</div>

Just as with how in C++ it is uncommon to need user-defined implementations for
copy and move constructors or user-defined implementations for destructors, in
Rust it is rare to need to implement the `Clone` and `Drop` traits by hand for
types that do not represent resources.

There is one exception to this. If the type has type parameters, it might be
desirable to implement `Clone` (and `Copy`) manually even if the clone should be
done field-by-field. See the [standard library documentation of
`Clone`](https://doc.rust-lang.org/std/clone/trait.Clone.html#how-can-i-implement-clone)
and [of
`Copy`](https://doc.rust-lang.org/std/marker/trait.Copy.html#how-can-i-implement-copy)
for details.

## Trivially copyable types

In C++, a class type is trivially copyable when it has no non-trivial copy
constructors, move constructors, copy assignment operators, move assignment
operators and it has a trivial destructor. Values of a trivially copyable type
are able to be copied by copying their bytes.

In the first C++ example above, `Age` is trivially copyable, but `Person` is
not. This is because despite using a default copy constructor, the constructor
is not trivial because `std::string` and `std::shared_ptr` are not trivially
copyable.

Rust indicates whether types are trivially copyable with the `Copy` trait. Just
as with trivially copyable types in C++, values of types that implement `Copy`
in Rust can be copied by copying their bytes. Rust requires explicit calls to
the `clone` method to make copies of values of types that do not implement
`Copy`.

In the first Rust example above, `Age` implements the `Copy` trait but `Person`
does not. This is because neither `std::String` nor `Rc<Person>` implement
`Copy`. They do not implement `Copy` because they own data that lives on the
heap, and so are not trivially copyable.

Rust prevents implementing `Copy` for a type if any of its fields are not
`Copy`, but does not prevent implementing `Copy` for types that should not be
copied bit-for-bit due to their intended meaning, which is usually indicated by
a user-defined `Clone` implementation.

Rust does not permit the implementation of both `Copy` and `Drop` for the same
type. This aligns with the C++ standard's requirement that trivially copyable
types not implement a user-defined destructor.

## Move constructors

In Rust, all types support move semantics by default, and custom move semantics
cannot be (and do not need to be) defined. This is because what "move" means in
Rust is not the same as it is in C++. In Rust, moving a value means changing
what owns the value. In particular, there is no "old" object to be destructed
after a move, because the compiler will prevent the use of a variable whose
value has been moved.

## Assignment operators

Rust does not have a copy or move assignment operator. Instead, assignment
either moves (by transferring ownership), explicitly clones and then moves, or
implicitly copies and then moves.

```rust
fn main() {
    let x = Box::<u32>::new(5);
    let y = x; // moves
    let z = y.clone(); // explicitly clones and then moves the clone
    let w = *y; // implicitly copies the content of the Box and then moves the copy
}
```

For situations where something like a user-defined copy assignment could avoid
allocations, the `Clone` trait has an additional method called `clone_from`. The
method is automatically defined, but can be overridden when implementing the
`Clone` trait to provide an efficient implementation. The default implementation
is the same as calling `Clone::clone` and performing a normal assignment.

The method is not used for normal assignments, but can be explicitly used in
situations where the performance of the assignment is significant and would be
improved by using the more efficient implementation, if one is defined. The
implementation can be made more efficient because `clone_from` takes ownership
of the object to which the values are being assigned, and so can do things like
reuse memory to avoid allocations.

```rust
fn go(x: &Vec<u32>) {
    let mut y = vec![0; x.len()];
    // ...
    y.clone_from(&x);
    // ...
}
```

## Performance concerns and `Copy`

The decision to implement `Copy` should be based on the semantics of the type,
not on performance. If the size of objects being copied is a concern, then one
should instead use a reference (`&T` or `&mut T`) or put the value on the heap
([`Box<T>`](https://doc.rust-lang.org/std/boxed/index.html) or
[`Rc<T>`](https://doc.rust-lang.org/std/rc/index.html)). These approaches
correspond to passing by reference, or using a `std::unique_ptr` or
`std::shared_ptr` in C++.

{{#quiz copy_and_move_constructors.toml}}
