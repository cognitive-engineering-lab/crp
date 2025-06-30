# Lambdas, closures, and function objects

## Function pointers

Both C++ and Rust permit the use of functions as values. In both, the values can
have [function pointer types](https://doc.rust-lang.org/std/primitive.fn.html).

<div class="comparison">

```cpp
#include <iostream>

int process(int n) {
  std::cout << n << std::endl;
  return 2 * n;
}

int main() {
  auto f = process;
  // or with type annotation
  // int (*f)(int) = process;

  std::cout << f(42) << std::endl;
}
```

```rust
fn process(n: i32) -> i32 {
    println!("{}", n);
    2 * n
}

fn main() {
    let f = process;
    // or with type annotation
    // let f: fn(i32) -> i32 = process;

    println!("{}", f(42));
}
```

</div>

Non-capturing closures are also convertible function pointers in both C++ and
Rust. In the following example the type could be inferred in both C++ and Rust,
but the type is explicitly given to demonstrate that in both cases the closure
has a function pointer type.

<div class="comparison">

```cpp
#include <iostream>

int main() {
  int (*f)(int) = [](int n) {
    std::cout << n << std::endl;
    return 2 * n;
  };

  std::cout << f(42) << std::endl;
}
```

```rust
fn main() {
    let f = |n: i32| {
        println!("{}", n);
        2 * n
    };
    // or with type annotation
    // let f: fn(i32) -> i32 = process;

    println!("{}", f(42));
}
```

</div>

Unlike in C++, in Rust functions can be defined within other functions. This has
the same meaning as defining the functions outside of the function (i.e., the
function is not a capturing closure and so cannot capture variables defined in
the outer function), but the name of the function is only available within the
outer function.

```rust
fn main() {
    fn process(n: i32) -> i32 {
        println!("{}", n);
        2 * n
    }

    println!("{}", process(42));
}
```

## Rust's call operator traits

In C++, any class can implement the call operator method `operator()` and be a
function object. Closures defined by lambdas do so automatically. In Rust the
equivalent are the call operator traits.

| Trait                                                                 | Method                                               | Description                                                                                |
|-----------------------------------------------------------------------|------------------------------------------------------|--------------------------------------------------------------------------------------------|
| [`FnOnce<Args>`](https://doc.rust-lang.org/std/ops/trait.FnOnce.html) | `fn call_once(self, args: Args) -> Self::Output`     | Can be called at most once                                                                 |
| [`FnMut<Args>`](https://doc.rust-lang.org/std/ops/trait.FnMut.html)   | `fn call_mut(&mut self, args: Args) -> Self::Output` | Can be called multiple times and may mutate captures (like the `mutable` specifier in C++) |
| [`Fn<Args>`](https://doc.rust-lang.org/std/ops/trait.Fn.html)         | `fn call(&self, args: Args) -> Self::Output`         | Can be called multiple times and do not mutate captures                                    |

Rust function pointers implement all three traits. Other closures implement the
traits depending on how they use the captured variables.

<details>
<summary>

Closure implementing only `FnOnce`

</summary>

<div class="comparison">

```cpp
#include <iostream>
#include <string>

int main() {
  std::string x("hi");
  auto f = [x = std::move(x)]() mutable {
    return std::move(x);
  };

  std::cout << f() << std::endl;
  // compiles, but the captured value has been
  // moved
  std::cout << f() << std::endl; // prints ""
}
```

```rust
fn main() {
   let f = {
       let x = String::from("hi");
       // f : FnOnce()
       move || x
   };

   // f() is equivalent to f.call_once()
   println!("{}", f()); // prints "hi"

   // Won't compile--call_once consumes f.
   // println!("{}", f());
}
```

</div>

</details>

<details>
<summary>

Closure implementing `FnMut` and taking ownership of the capture

</summary>

<div class="comparison">

```cpp
#include <string>

int main() {
  std::string x("");
  auto f = [x = std::move(x)]() mutable {
    x.push_back('!');
    return x.size();
  };

  std::cout << f() << std::endl; // prints "1"
  std::cout << f() << std::endl; // prints "2"
}
```

```rust
fn main() {
    let mut f = {
        let mut x = String::from("");
        // f : FnMut() -> usize
        move || {
            x.push('!');
            x.len()
        }
    };

    println!("{}", f()); // prints "1"
    println!("{}", f()); // prints "2"
}
```

</div>

</details>

<details>
<summary>

Closure implementing `FnMut` and capturing by mutable reference

</summary>

In this case, `x` has to be alive as long as the closure might be used, since
the closure borrows `x`. Therefore, `x` can't be declared in a block with the
lambda like in the previous example.

<div class="comparison">

```cpp
#include <iostream>
#include <string>

int main() {
  std::string x("");
  auto f = [&x]() {
    x.push_back('!');
    return x.size();
  };

  std::cout << f() << std::endl; // prints "1"
  std::cout << f() << std::endl; // prints "2"
}
```

```rust
fn main() {
    let mut x = String::from("");
    // g : FnMut() -> usize
    let mut f = || {
        x.push('!');
        x.len()
    };

    println!("{}", f()); // prints "1"
    println!("{}", f()); // prints "2"
}
```

</div>

</details>

<details>
<summary>

Closure implementing `Fn`

</summary>

Whether `x` is `mut` or not doesn't affect whether the closure implements `Fn`
or `FnMut`. What matters is how `x` is used by the closure.

<div class="comparison">

```cpp
#include <iostream>
#include <string>

int main() {
  std::string x("");
  auto f = [&x]() { return x.size(); };

  std::cout << f() << std::endl; // prints "0"
  std::cout << f() << std::endl; // prints "0"
}
```

```rust
fn main() {
    let f = {
        let x = String::from("");
        // g : Fn() -> usize
        move || x.len()
    };

    println!("{}", f()); // prints "0"
    println!("{}", f()); // prints "0"
}
```

</div>

</details>

## Lambdas and closures

In neither C++ nor Rust can the concrete type of a capturing closure be written.
In both languages, for local variables this means that the type must be
inferred.

<div class="comparison">

```cpp
#include <iostream>
#include <sstream>
#include <string>

int main() {
  std::string greeting = "hello";
  // Can't write the type of the closure
  auto sayHelloTo = [&](std::string &who) {
    std::ostringstream out;
    out << greeting << " " << who;
    return out.str();
  };

  std::string world("world");
  std::string moon("moon");

  std::cout << sayHelloTo(world) << std::endl;
  std::cout << sayHelloTo(moon) << std::endl;
}
```

```rust
fn main() {
    let greeting = "hello";

    // Can't write the type of the closure
    let say_hello_to = |who: &str| {
        format!("{} {}", greeting, who)
    };

    println!("{}", say_hello_to("world"));
    println!("{}", say_hello_to("moon"));
}
```

</div>

In both C++ and Rust if the closure is heap-allocated a type can be given. In
C++ this is done using `std::function` while in Rust it again is done with the
call operator traits.

<div class="comparison">

```cpp
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

int main() {
  std::string greeting = "hello";
  // Can't write the type of the closure
  std::function<std::string(std::string &)>
      sayHelloTo([&](std::string &who) {
        std::ostringstream out;
        out << greeting << " " << who;
        return out.str();
      });

  std::string world("world");
  std::string moon("moon");

  std::cout << sayHelloTo(world) << std::endl;
  std::cout << sayHelloTo(moon) << std::endl;
}
```

```rust
fn main() {
    let greeting = "hello";

    // Can't write the type of the closure
    let say_hello_to: Box<
        dyn Fn(&str) -> String,
    > = Box::new(|who: &str| {
        format!("{} {}", greeting, who)
    });

    println!("{}", say_hello_to("world"));
    println!("{}", say_hello_to("moon"));
}
```

</div>

Since `std::function` can be empty the above example isn't strictly equivalent.
However, since `std::function` is often used with the side condition that the
value not be empty, the `Box` without an `Option` wrapper for representing the
empty case is the more practical translation.

A type can also be given in terms of one of the function operator traits for
references to closures in Rust.

```rust
fn main() {
    let greeting = "hello";

    // Can't write the type of the closure
    let say_hello_to = |who: &str| {
        format!("{} {}", greeting, who)
    };

    let say: &dyn Fn(&str) -> String = &say_hello_to;

    println!("{}", say("world"));
    println!("{}", say("moon"));
}
```

Additionally, in both C++ and Rust, the return type of the closure can be
annotated as part of the lambda expression. This is useful when the return type
either cannot be inferred or should be less specific than what would be
inferred. In the following example this is used to return a value in terms of an
interface it implements instead of the concrete type that would otherwise be
inferred.

<div class="comparison">

```cpp
#include <iostream>
#include <memory>
#include <string>

// The common interface
struct Debug {
  virtual std::ostream &
  emit(std::ostream &out) const = 0;
};

std::ostream &operator<<(std::ostream &out,
                         const Debug &d) {
  d.emit(out);
  return out;
}

// Two things that implement the interface
struct A : public Debug {
  std::ostream &
  emit(std::ostream &out) const override {
    out << "A";
    return out;
  }
};

struct B : public Debug {
  std::ostream &
  emit(std::ostream &out) const override {
    out << "B";
    return out;
  }
};

int main() {
  // Without the return-type annotation,
  // std::unique_ptr<A> would be inferred.
  auto f = [](std::unique_ptr<A> a,
              std::unique_ptr<B> b)
      -> std::unique_ptr<Debug> { return a; };
  std::cout << *f(std::make_unique<A>(),
                  std::make_unique<B>())
            << std::endl;
}
```

```rust
// The common interface
use std::fmt::Debug;

// Two things that implement the interface
#[derive(Debug)]
struct A;

#[derive(Debug)]
struct B;

fn main() {
    // Without the return type annotation,
    // Box<A> would be inferred.
    let f = move |a: Box<A>,
                  b: Box<B>|
          -> Box<dyn Debug> { a };
    println!("{:?}", f(Box::new(A), Box::new(B)));
}
```

</div>

## Capturing variables

In C++, capture specifiers are used to indicate whether a variable should be
captured by reference, by copy, or by move. The capture specifiers can be given
for all of the variables at once, for each variable, or given as a default along
with specific choices for each variable.

In Rust, the variables are captured either all by reference (by default) or all by move (using
a `move` specifier). In order to express other capture strategies, the references
and copies need to be explicitly defined and the closure needs to capture those
variables instead.

Expressing the pattern of explicitly making copies or taking references
leverages the fact that in Rust blocks are expressions. In the examples that
need to do that, notice the lack of a semicolon in the last statement of the
block that is being assigned to the variable to hold the closure.

The following examples show examples of different patterns of capturing
variables in C++ and their analogs in Rust.

<details>
<summary>

Capture `x` and `y` by reference

</summary>
<div class="comparison">

```cpp
#include <iostream>
#include <string>

int main() {
  std::string x("hello world");
  std::string y("goodnight moon");

  auto f = [&]() {
    std::cout << x << std::endl;
    std::cout << y << std::endl;
  };

  // x and y borrowed by f, but still available
  std::cout << x << std::endl;
  std::cout << y << std::endl;

  f();
}
```

```rust
fn main() {
    let x = String::from("hello world");
    let y = String::from("goodnight moon");

    let f = || {
        println!("{}", x);
        println!("{}", y);
    };

    // x and y borrowed by f, but still available
    println!("{}", x);
    println!("{}", y);

    f();
}
```

</div>
</details>

<details>
<summary>

Capture `x` and `y` by mutable reference

</summary>

The C++ version is same as when capturing by mutable reference.

<div class="comparison">

```cpp
#include <iostream>
#include <string>

int main() {
  std::string x("hello world");
  std::string y("goodnight moon");

  auto f = [&]() {
    x.push_back('!');
    y.push_back('!');
  };

  // x and y borrowed by mutably f, but still
  // available anyway
  std::cout << x << std::endl;
  std::cout << y << std::endl;

  f();

  std::cout << x << std::endl;
  std::cout << y << std::endl;
}
```

```rust
fn main() {
    let mut x = String::from("hello world");
    let mut y = String::from("goodnight moon");

    // f needs to be mut because it mutates
    // its captured variables
    let mut f = || {
        x.push('!');
        y.push('!');
    };

    // x and y borrowed mutably by f, and so
    // can't be used here
    // println!("{}", x);
    // println!("{}", y);

    f();

    println!("{}", x);
    println!("{}", y);
}
```

</div>
</details>

<details>
<summary>

Copy `x` and `y` to capture by value

</summary>

In C++ this requires that the lambda have the `mutable` specifier. In Rust this
requires

- making a copy of the values for the closure to capture,
- marking those copy as mutable with `mut`,
- marking the closure itself as mutable with `mut`, and
- using the `move` specifier to move ownership of the copies into the closure.

Types that indicate they are [trivially copyable by implementing the `Copy`
trait](constructors/copy_and_move_constructors.md#trivially-copyable-types) do
not need to be explicitly cloned.

<div class="comparison">

```cpp
#include <iostream>
#include <string>

int main() {
  std::string x("hello world");
  std::string y("goodnight moon");

  auto f = [=]() mutable {
    x.push_back('!');
    y.push_back('!');
  };

  // copies of x and y owned by f, originals
  // still available
  std::cout << x << std::endl;
  std::cout << y << std::endl;

  f();

  // still don't have the !, since the copies
  // were modified not the originals
  std::cout << x << std::endl;
  std::cout << y << std::endl;
}
```

```rust
fn main() {
    let x = String::from("hello world");
    let y = String::from("goodnight moon");

    let mut f = {
        // Shadow outer variables with copies.
        // This needs to happen outside of the
        // closure expression.
        let mut x = x.clone();
        let mut y = y.clone();
        move || {
            x.push('!');
            y.push('!');
        }
    };

    // clones of x and y owned by f, originals
    // still available
    println!("{}", x);
    println!("{}", y);

    f();

    // still don't have the !, since the copies
    // were modified not the originals
    println!("{}", x);
    println!("{}", y);
}
```

</div>
</details>

<details>
<summary>

Move `x` and `y` to capture by value

</summary>

<div class="comparison">

```cpp
#include <iostream>
#include <string>

int main() {
  std::string x("hello world");
  std::string y("goodnight moon");

  auto f = [x = std::move(x),
            y = std::move(y)]() {
    std::cout << x << std::endl;
    std::cout << y << std::endl;
  };

  // x and y moved into f,
  // empty strings left behind.
  std::cout << x << std::endl;
  std::cout << y << std::endl;

  f();
}
```

```rust
fn main() {
    let x = String::from("hello world");
    let y = String::from("goodnight moon");

    // captures x and y by value
    let f = move || {
        println!("{}", x);
        println!("{}", y);
    };

    // x and y moved into f,
    // original variables cannot be used
    // println!("{}", x);
    // println!("{}", y);

    f();
}
```

</div>
</details>

<details>
<summary>

Move `x` to capture by value, capture `y` by reference

</summary>

<div class="comparison">

```cpp
#include <iostream>
#include <string>

int main() {
  std::string x("hello world");
  std::string y("goodnight moon");

  auto f = [x = std::move(x), &y]() mutable {
    x.push_back('!');
    std::cout << x << std::endl;
    std::cout << y << std::endl;
  };

  // x moved into f, y borrowed by f
  std::cout << x << std::endl;
  std::cout << y << std::endl;

  f();
}
```

```rust
fn main() {
    let mut x = String::from("hello world");
    let y = String::from("goodnight moon");

    let mut f = {
        let y = &y;
        // Actually captures both x and y by
        // value, but y is a reference
        move || {
            x.push('!');
            println!("{}", x);
            println!("{}", y);
        }
    };

    // x moved into f, y borrowed by f
    // println!("{}", x);
    println!("{}", y);

    f();
}
```

</div>
</details>

## Function objects

Unlike in C++, in Rust only functions and closures implement the function call
operator traits. The ability to directly implement the traits is [not yet part
of stable Rust](https://github.com/rust-lang/rust/issues/29625).

Instead, one can implement a [conversion
function](./user-defined_conversions.md). The standard conversion traits `From`
and `Into` cannot be implemented for this purpose, however, because the `impl
Trait` syntax cannot be used in trait implementations.[^impl-trait-impl] Instead
a separate method must be defined.

[^impl-trait-impl]: That is, one can write neither `impl From<&MyClosure> for
    impl Fn() -> usize {...}` nor `impl Into<impl Fn() -> usize> for &MyClosure
    {...}`.

<div class="comparison">

```cpp
#include <cstddef>
#include <iostream>
#include <string>

struct MyClosure {
  std::string msg;

  std::size_t operator()() {
    std::cout << msg << std::endl;
    return msg.size();
  }
};

int main() {
  MyClosure myClosure{"hello world"};
  myClosure();
}
```

```rust,edition2024
struct MyClosure {
    msg: String,
}

impl MyClosure {
    fn as_fn(&self) -> impl Fn() -> usize {
        move || {
            println!("{}", self.msg);
            self.msg.len()
        }
    }
}

fn main() {
    let my_closure = MyClosure {
        msg: String::from("hello world"),
    };
    let f = my_closure.as_fn();

    f();
}
```

</div>

In Rust editions earlier than 2024, the above example requires a precise
capturing annotation using the [`use<'a>`
syntax](https://doc.rust-lang.org/std/keyword.use.html#precise-capturing) to
specify that the returned closure borrows from the parameters, since otherwise a
lifetime bound is not inferred.

## Member functions as function pointers

In C++, pointers to member functions can be invoked with the `.*` operator or
can be converted to `std::function` values using `std::mem_fn`, enabling them to
be used in the same way as other `std::function` values. When called on a
derived class, whether the method whose address was taken or the overriding
method in the derived class is called depends on whether the method is defined
as virtual.

In Rust pointers to member functions are normal function pointers. For example,
a method on a type `T` with a `&self` parameter is a function whose first
argument has type `&T`. When the method is named via a trait, then the first
argument of the function has type `&dyn T` with a lifetime bound. Determining
whether vtables are involved in the use of a pointer to a member function is
determined at the time that the method is referenced, rather than when the
method is defined.

<div class="comparison">

```cpp
#include <cassert>
#include <functional>
#include <iostream>

struct Interface {
  virtual void showVirtual() = 0;
};

struct A : public Interface {
  void show() {
    std::cout << "A" << std::endl;
  }

  void showVirtual() override {
    std::cout << "A" << std::endl;
  }
};

struct B : public Interface {
  void showVirtual() override {
    std::cout << "B" << std::endl;
  }

  void show() {
    std::cout << "B" << std::endl;
  }
};

int main() {
  auto showV = &Interface::showVirtual;
  auto showA = &A::show;
  auto showB = &B::show;

  A a;
  B b;

  (a.*showV)(); // prints A
  (b.*showV)(); // prints B

  (a.*showA)(); // prints A
  (b.*showB)(); // prints B
}
```

```rust
trait Interface {
    fn show(&self);
}

struct A;

impl Interface for A {
    fn show(&self) {
        println!("A");
    }
}

struct B;

impl Interface for B {
    fn show(&self) {
        println!("B");
    }
}

fn main() {
    // types could be inferred, but given to show
    // that they are just a function pointers
    let show_a: fn(&A) = A::show;
    let show_b: fn(&B) = B::show;
    let show_v: fn(&(dyn Interface + 'static)) =
        Interface::show;

    show_a(&A); // prints A
    show_b(&B); // prints B

    show_v(&A); // prints A
    show_v(&B); // prints B
}
```

</div>

## Closures as parameters

In both C++ and Rust, unboxed closures can be accepted as parameters. Just as
using `auto` as the type of a parameter in C++ makes the function actually a
function template, using `impl Trait` as the type of a parameter in Rust makes
the function generic. [The resulting generic function is checked statically,
just like it would be if the type parameter and bound were given
explicitly.](data_modeling/concepts.md#templates-vs-generic-functions)

<div class="comparison">

```cpp
#include <iostream>

int apply_to_0(auto f) {
  return f(0);
}

int main() {
  int x = 1;
  auto f([=](int n) { return n + x; });
  std::cout << apply_to_0(f) << std::endl;
}
```

```rust
fn apply_to_0(f: impl FnOnce(i32) -> i32) -> i32 {
    f(0)
}

fn main() {
    let x = 1;
    let f = move |n: i32| x + n;
    println!("{}", apply_to_0(&f));
}
```

</div>

When accepting closures as type parameters in Rust, it is best practice to
specify the type as the the least restrictive interface required for how the
closure will be used.

Using `FnOnce` as the bound is the least restrictive, and so should be used so
that the function accepting a closure as a parameter is as compatible with as
many closures as possible . `FnOnce` works with `Fn` and `FnMut` because there
are `FnOnce` trait implementations for `&Fn` and `&FnMut`. The `FnMut` trait is
the next most restrictive, followed by `Fn`, and then actual function pointers,
whose types are written with a lowercase `fn`.

In both C++ and Rust, it is also possible to pass references or pointers to closures. In
the following example, the closure is in dynamically allocated storage in both
C++ and in Rust.

<div class="comparison">

```cpp
#include <functional>
#include <iostream>

int apply_to_0(std::function<int(int)> f) {
  return f(0);
}

int main() {
  int x = 1;
  // closure is on heap
  auto f(std::function(
      [=](int n) { return n + x; }));
  std::cout << apply_to_0(f) << std::endl;
}
```

```rust
fn apply_to_0(f: Box<dyn FnOnce(i32) -> i32>) -> i32 {
    f(0)
}

fn main() {
    let x = 1;
    let f = Box::new(move |n: i32| x + n);
    println!("{}", apply_to_0(f));
}
```

</div>

`FnOnce` can be called when in a `Box`, because the box owns the trait object,
but not when in a reference which doesn't. `Fn` and `FnMut` do not have the same
restriction.

## Returning closures

In C++, `auto` or `decltype(auto)` can be used as the return type for a function
returning a closure. In Rust, once again the `impl Trait` syntax can be used.
Just as how in C++ using `auto` in this way does not denote an abbreviated
function template, it does not denote a generic function in Rust. Instead the
type is inferred, and must satisfy the trait.

<div class="comparison">

```cpp
#include <iostream>

decltype(auto) makeConst(int n) {
  return [n]() { return n; };
}

int main() {
  auto f = makeConst(42);
  std::cout << f() << std::endl;
}
```

```rust
fn make_const(n: i32) -> impl Fn() -> i32 {
    move || n
}

fn main() {
    let f = make_const(42);
    println!("{}", f());
}
```

</div>

In places in C++ where `decltype` is used to name the closure, e.g., when
returning a closure in a template class, in Rust the `impl Trait` syntax is
used. If a type needs to be given in a let binding, then an underscore `_` can
be used to indicate that the part of the type that is the closure's type should
be inferred.

```rust
struct Wrapper<T>(T);

fn make_closure() -> Wrapper<impl Fn(i32) -> i32>
{
    let x = 1;
    Wrapper(move |n: i32| x + n)
}

fn main() {
    let w: Wrapper<_> = make_closure();
    w.0(0);
}
```

There are several other places where `decltype` works but `impl Trait` does not
yet, such as [the output type for `Fn`
traits](https://github.com/rust-lang/rust/issues/99697). This means that one can
define closures that return closures in Rust, but cannot give them a type, and
therefore cannot return them from functions. The following compiles in C++ but
fails to compile in Rust for that reason.

<div class="comparison">

```cpp
decltype(auto) makeClosure(int n) {
  return [n]() { return [n]() { return n; }; };
}
```

```rust,ignore
// Does not compile: not yet supported
fn make_closure(
    n: i32,
) -> impl Fn() -> impl Fn() -> i32 {
    move || move || n
}
```

</div>

## Template lambdas

Rust does not support generic closures. Thus, the following has no equivalent in
Rust.

```cpp
#include <string>

int main() {
  int n = 0;

  auto idCounter = [&]<typename T>(T x) {
    n++;
    return x;
  };

  int y = idCounter(5);
  std::string z =
      idCounter.template operator()<std::string>(std::string("hi"));
}
```

However, if the lambda doesn't capture anything, it is possible to write the
following equivalent in Rust, by using an inner function definition.

<div class="comparison">

```cpp
#include <string>

int main() {
  auto id = []<typename T>(T x) { return x; };
  int y = id(5);
  std::string z = id(std::string("hi"));
}
```

```rust
fn main() {
    fn id<T>(x: T) -> T {
        x
    }

    id(5);
    id(String::from("hi"));
}
```

</div>

## Partial application and `std::bind`

There is no equivalent to the C++ template `std::bind` in the Rust standard
library. The idiomatic way to express partial application in Rust is to write
out the lambda.

<div class="comparison">

```cpp
#include <cassert>
#include <functional>

int add(int x, int y) {
  return x + y;
}

int main() {
  using namespace std::placeholders;

  auto addTen = std::bind(add, 10, _1);
  assert(42 == addTen(32));
}
```

```rust
fn add(x: i32, y: i32) -> i32 {
    x + y
}

fn main() {
    let add_ten = move |y| add(10, y);
    assert_eq!(42, add_ten(32));
}
```

</div>

The third-party crate
[partial\_application](https://docs.rs/partial_application/latest/partial_application/)
provides something akin to `std::bind` using Rust macros.

```rust,ignore
use partial_application::*;

fn add(x: i32, y: i32) -> i32 {
    x + y
}

fn main() {
    let add_ten = partial!(move add => 10, _);

    assert_eq!(42, add_ten(32));
}
```

## Returning references to captured variables

In Rust it is not possible to have a closure return a reference to a captured
variable. This is due to a limitation with how the `Fn` family of traits are
defined: `Fn::Output` does not have a way to express a lifetime bound.

<div class="comparison">

```cpp
#include <iostream>
#include <string>

int main() {
  std::string msg("hello world");
  auto f = [=]() -> const std::string & {
    return msg;
  };
  std::cout << f() << std::endl;
}
```

```rust,ignore
fn main() {
    let msg = String::from("hello world");
    // fails to compile!
    let f = move || &msg;

    println!("{}", f());
}
```

</div>

The workarounds to this limitation in Rust involve either heap-allocating and
using a shared pointer `Rc`, or defining a new trait instead of using one of the
`Fn` traits. The following example shows a trait that uses [generic associated types](https://doc.rust-lang.org/reference/items/associated-items.html#r-items.associated.type.generic) to define a generalized
`Fn` trait. In practice, however, it is usually better either to
define a custom trait for each use case or to elide the trait entirely if a
single struct is sufficient.

```rust
trait Closure<Args> {
    // The lifetime parameter enables expressing
    // the bound.
    type Output<'a>
    where
        Self: 'a;

    // The bound from self can then be
    // provided to Output.
    fn call<'a>(
        &'a self,
        args: Args,
    ) -> Self::Output<'a>;
}

struct MyClosure {
    msg: String,
}

impl Closure<()> for MyClosure {
    type Output<'a> = &'a str;

    fn call(&self, _: ()) -> &str {
        &self.msg
    }
}

fn main() {
    let f = MyClosure {
        msg: String::from("hello world"),
    };

    println!("{}", f.call(()));
}
```

## Closures, ownership, and `FnOnce`

Closures are a part of Rust where the borrow checker is likely to cause
frustration for a C++ programmer. This is usually not because of lifetimes,
which have to be similarly considered in C++, but rather because C++ defaults to
copy semantics while Rust defaults to move semantics. For example, this small
adjustment to one of the earlier examples fails to compile.

```rust,ignore
fn main() {
    let greeting = "hello ".to_string();

    // Can't write the type of the closure
    let say_hello_to = move |who: &str| {
        greeting + who
    };

    println!("{}", say_hello_to("world"));
    println!("{}", say_hello_to("moon"));
}
```

This fails to compile because the `+` operator takes ownership of `greeting`,
which makes it no longer accessible for later invocations. Because of this, the
closure only implements `FnOnce`, not `Fn`, and therefore can only be called
once, because the call takes ownership of the closure itself.

```text
error[E0382]: use of moved value: `say_hello_to`
 --> example.rs:9:20
  |
8 |     println!("{}", say_hello_to("world"));
  |                    --------------------- `say_hello_to` moved due to this call
9 |     println!("{}", say_hello_to("moon"));
  |                    ^^^^^^^^^^^^ value used here after move
  |
note: closure cannot be invoked more than once because it moves the variable `greeting` out of its environment
 --> example.rs:6:26
  |
6 |         move |who: &str| greeting + who;
  |                          ^^^^^^^^
note: this value implements `FnOnce`, which causes it to be moved when called
 --> example.rs:8:20
  |
8 |     println!("{}", say_hello_to("world"));
  |
```

In many cases like this, the answer is to clone the value so that the copy owned
by the closure can be retained for future invocations.

```rust
fn main() {
    let greeting = "hello ".to_string();

    // Can't write the type of the closure
    let say_hello_to = move |who: &str| {
        greeting.clone() + who
    };

    println!("{}", say_hello_to("world"));
    println!("{}", say_hello_to("moon"));
}
```

If cloning isn't desired because it is too expensive, then the closure needs to
be redesigned to avoid giving away ownership of its captured variables.

## Documentation best practices

C++ often recommended to explicitly list captures in a lambda expression,
especially in situations where a closure will outlive its context. The purpose
of this is to assist in reasoning about the lifetimes of the captures to ensure
that the closure does not outlive any of the objects it has captured.

In Rust, the same decisions about captures with respect to lifetimes have to be
made, but the compiler tracks them instead of having to do the reasoning
manually. That is, in spite of the type of the closure not being expressible, it
does still include the lifetimes of variables captured by reference, and so is
checked the same way that any other structure would be.

This results in the best practices for documenting closures in Rust not
including enumerating captures, even in situations where one would do so in C++.

The same is true about the destructibility of the content of the captures in a
closure. The example involving `FnOnce` functions in [the previous
section](#closures-ownership-and-fnonce) may be a point of frustration
initially, but the behavior has the benefit of reducing the documentation and reasoning
burdens.

{{#quiz closures.toml}}
