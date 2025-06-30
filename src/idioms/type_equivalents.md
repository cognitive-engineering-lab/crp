# Type equivalents

The type equivalents listed in this document are equivalent for the purposes of
programming in Rust as one would program in C++. They are not necessarily
equivalent in terms of being useful for interacting with C or C++ programs via
an FFI. For types that are useful for interoperability with C or C++, see the
[Rust `std::ffi` module
documentation](https://doc.rust-lang.org/std/ffi/index.html) and the [FFI
documentation in the Rustonomicon](https://doc.rust-lang.org/nomicon/ffi.html).

## Primitive types

### Integer types

In C++, many of the integer types (like `int` and `long`) have implementation
defined widths. In Rust, integer types are always specified with their widths,
much like the types in `<cstdint>` in C++. When it isn't clear what integer type
to use, [it is common to default to `i32`, which is the type that Rust defaults
to for integer
literals](https://doc.rust-lang.org/book/ch03-02-data-types.html#integer-types).

| C++ type    | Rust type |
|-------------|-----------|
| `uint8_t`   | `u8`      |
| `uint16_t`  | `u16`     |
| `uint32_t`  | `u32`     |
| `uint64_t`  | `u64`     |
| `int8_t`    | `i8`      |
| `int16_t`   | `i16`     |
| `int32_t`   | `i32`     |
| `int64_t`   | `i64`     |
| `size_t`    | `usize`   |
| `ptrdiff_t` | `isize`   |

In C++ `size_t` is conventionally used only indexing, sizes, and offsets. The
same is true in Rust for `usize`, which is the pointer-sized unsigned integer
type. The pointer-sized signed integer type `isize` follows similar conventions.

Some primitive C++ and POSIX types (such as `ssize_t` and `off_t` as return
types) do not map to `isize` because the failure case is representing using
`std::io::Result` instead of using a negative number as a [sentinel
value](./null/sentinel_values.md). Others (such as `fpos_t` or `off_t` as
a parameter type) are represented as a plain `u64` or have a more explicit
representation in Rust.

| C++ type                       | Rust type                                                                   |
|--------------------------------|-----------------------------------------------------------------------------|
| POSIX `ssize_t`                | [`std::io::Result<u64>`](https://doc.rust-lang.org/std/io/type.Result.html) |
| POSIX `off_t` (as argument)    | `u64`                                                                       |
| POSIX `off_t`(as return value) | [`std::io::Result<u64>`](https://doc.rust-lang.org/std/io/type.Result.html) |
| `fpos_t`                       | [`std::io::SeekFrom`](https://doc.rust-lang.org/std/io/enum.SeekFrom.html)  |

### Floating point types

As with integer types in C++, the floating point types `float`, `double`, and
`long double` have implementation defined widths. C++23 introduced types
guaranteed to be IEEE 754 floats of specific widths. Of those, `float32_t` and
`float64_t` correspond to what is usually expected from `float` and `double`.
Rust's floating point types are analogous to these.

| C++ type     | Rust type |
|--------------|-----------|
| `float16_t`  |           |
| `float32_t`  | `f32`     |
| `float64_t`  | `f64`     |
| `float128_t` |           |

The Rust types analogous to `float16_t` and `float128_t` (`f16` and `f128`) are
[not yet available in stable
Rust](https://github.com/rust-lang/rust/issues/116909).

### Raw memory types

In C++ pointers to or arrays of `char`, `unsigned char`, or `byte` are used to
represent raw memory. In Rust, arrays (`[u8; N]`), vectors (`Vec<u8>`), or
slices (`&[u8]`) of `u8` are used to accomplish the same goal. However,
accessing the underlying memory of another Rust value in that way requires
unsafe Rust. There are [libraries](../etc/libraries.md) for creating safe wrappers
around that kind of access for purposes such as serialization or interacting
with hardware.

### Character and string types

The C++ `char` or `wchar_t` types have implementation defined widths. Rust does
not have an equivalent to these types. When working with string encodings in
Rust one would use unsigned integer types where one would use the fixed width
character types in C++.

| C++ type   | Rust type |
|------------|-----------|
| `char8_t`  | `u8`      |
| `char16_t` | `u16`     |

The Rust `char` type represents a Unicode scalar value. Thus, a Rust `char` is
the same size as a `u32`. For working with characters in Rust strings (which are
guaranteed to be valid UTF-8), the `char` type is appropriate. For representing
a byte, one should instead use `u8`.

The Rust standard library includes a type for UTF-8 strings and string slices:
`String` and `&str`, respectively. Both types guarantee that represented strings
are valid UTF-8. The Rust `char` type is appropriate for representing elements
of a `String`.

Because `str` (without the reference) is a slice, it is unsized and therefore
must be used behind a pointer-like construct, such as a reference or box. For
this reason, string slices are often described as `&str` instead of `str` in
documentation, even though they can also be used as `Box<str>`, `Rc<str>`, etc.

Rust also includes types for platform-specific string representations and slices
of those strings:
[`std::ffi::OsString`](https://doc.rust-lang.org/std/ffi/struct.OsString.html)
and `&std::ffi::OsStr`. While these strings use the OS-specific representation,
to use one with the Rust FFI, it must still be converted to a
[`CString`](https://doc.rust-lang.org/std/ffi/struct.CString.html).

Unlike C++ which has `std::u16string`, Rust has no specific representation for
UTF-16 strings. Something like `Vec<u16>` can be used, but the type will not
guarantee that its contents are a valid UTF-16 string. Rust does provide a
mechanisms for converting `String` to and from a UTF-16 encoding
([`String::encode_utf16`](https://doc.rust-lang.org/std/string/struct.String.html#method.encode_utf16)
and
[`String::from_utf16`](https://doc.rust-lang.org/std/string/struct.String.html#method.from_utf16),
among others) as well as similar mechanisms for accessing the underlying UTF-8
encoding
(https://doc.rust-lang.org/std/string/struct.String.html#method.from_utf8).

| Purpose             | Rust type                          |
|---------------------|------------------------------------|
| representing text   | `String` and `&str`                |
| representing bytes  | vectors, arrays, or slices of `u8` |
| interacting with OS | `OsString` and `&OsStr`            |
| representing UTF-8  | `String`                           |
| representing UTF-16 | use [a library](../etc/libraries.md) |

### Boolean types

The `bool` type in Rust is analogous to the `bool` type in C++. Unlike C++, Rust
makes [guarantees about the size, alignment, and bit pattern used to represent
values of the `bool`
type](https://doc.rust-lang.org/reference/types/boolean.html).

### `void`

In C++ `void` indicates that a function does not return a value. Because Rust is
expression-oriented, all functions return values. In the place of `void`, Rust
uses the unit type `()`. When a function does not have a return type declared,
`()` is the return type.

<div class="comparison">

```cpp
#include <iostream>

void process() {
    std::cout
        << "Does something, but returns nothing."
        << std::endl;
}
```

```rust
fn process() {
    println!("Does something but returns nothing.");
}
```

</div>

Since the unit type has only one value (also written `()`), values of the type
provide no information. This also means that the return value can be left
implicit, as in the above example. The following example makes the unit type
usage explicit.

```rust
fn process() -> () {
    let () = println!("Does something but returns nothing.");
    ()
}
```

The syntax of the unit type and syntax of the unit value resemble that of an
empty tuple. Essentially, that is what the type is. The following example shows
some equivalent types, though without the special syntax or language
integration.

```rust
struct Pair<T1, T2>(T1, T2); // the same as (T1, T2)
struct Single<T>(T); // a tuple with just one value (T1)
struct Unit; // the same as ()
// can also be written as
// struct Unit();

fn main() {
    let pair = Pair(1,2.0);
    let single = Single(1);
    let unit = Unit;
    // can also be written as
    // let unit = Unit();
}
```

Using a unit type instead of `void` enables expressions with unit type (such as
function calls that would return `void` in C++) to be used in contexts that
expect a value. This is especially helpful with defining and using generic
functions, instead of needing something like `std::is_void` to special-case the
handling when a type is `void`.

## Pointers

The following table maps the ownership-managing classes from C++ to equivalents
types in Rust.

| Use                                                       | C++ type                                 | Rust type                                                                                                                                                                    |
|-----------------------------------------------------------|------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Owned                                                     | `T`                                      | `T`                                                                                                                                                                          |
| Single owner, dynamic storage                             | `std::unique_ptr<T>`                     | `Box<T>`                                                                                                                                                                     |
| Shared owner, dynamic storage, immutable, not thread-safe | `std::shared_ptr<T>`                     | `std::rc::Rc<T>`                                                                                                                                                             |
| Shared owner, dynamic storage, immutable, thread-safe     | `std::shared_ptr<T>`                     | `std::sync::Arc<T>`                                                                                                                                                          |
| Shared owner, dynamic storage, mutable, not thread-safe   | `std::shared_ptr<T>`                     | [`std::rc::Rc<std::cell::RefCell<T>>`](https://doc.rust-lang.org/book/ch15-05-interior-mutability.html#having-multiple-owners-of-mutable-data-by-combining-rct-and-refcellt) |
| Shared owner, dynamic storage, mutable, thread-safe       | `std::shared_ptr<T>` with a `std::mutex` | [`std::sync::Arc<std::mutex::Mutex<T>>`](https://doc.rust-lang.org/book/ch16-03-shared-state.html)                                                                           |
| Const reference                                           | `const &T`                               | `&T`                                                                                                                                                                         |
| Mutable reference                                         | `&T`                                     | `&mut T`                                                                                                                                                                     |
| Const observer pointer                                    | `const *T`                               | `&T`                                                                                                                                                                         |
| Mutable observer pointer                                  | `*T`                                     | `&mut T`                                                                                                                                                                     |

In C++, the thread safety of `std::shared_ptr` is more nuanced than it appears
in this table (e.g., some uses may require `std::atomic`). However, in safe Rust
the compiler will prevent the incorrect use of the shared owner types.

Unlike with C++ references, Rust can have references-to-references. Rust
references are more like observer pointers than they are like C++ references.

### `void*`

Rust does not have anything directly analogous to `void*` in C++. The upcoming chapter
on `RTTI`<!--LINKME--> will cover some use cases where the goal is dynamic
typing. The [FFI chapter of the
Rustonomicon](https://doc.rust-lang.org/nomicon/ffi.html#representing-opaque-structs)
covers some use cases where the goal is interoperability with C programs that
use `void*`.

## Containers

Both C++ and Rust containers own their elements. However, in both the element
type may be a non-owning type, such as a pointer in C++ or a reference in Rust.

| C++ type                  | Rust type                                                                                             |
|---------------------------|-------------------------------------------------------------------------------------------------------|
| `std::vector<T>`          | [`Vec<T>`](https://doc.rust-lang.org/std/vec/struct.Vec.html)                                         |
| `std::array<T, N>`        | [`[T; N]`](https://doc.rust-lang.org/std/primitive.array.html)                                        |
| `std::list<T>`            | [`std::collections::LinkedList<T>`](https://doc.rust-lang.org/std/collections/struct.LinkedList.html) |
| `std::queue<T>`           | [`std::collections::VecDeque<T>`](https://doc.rust-lang.org/std/collections/struct.VecDeque.html)     |
| `std::deque<T>`           | [`std::collections::VecDeque<T>`](https://doc.rust-lang.org/std/collections/struct.VecDeque.html)     |
| `std::stack<T>`           | [`Vec<T>`](https://doc.rust-lang.org/std/vec/struct.Vec.html)                                         |
| `std::map<K,V>`           | [`std::collections::BTreeMap<K,V>`](https://doc.rust-lang.org/std/collections/struct.BTreeMap.html)   |
| `std::unordered_map<K,V>` | [`std::collections::HashMap<K,V>`](https://doc.rust-lang.org/std/collections/struct.HashMap.html)     |
| `std::set<K>`             | [`std::collections::BTreeSet<K>`](https://doc.rust-lang.org/std/collections/struct.BTreeSet.html)     |
| `std::unordered_set<K>`   | [`std::collections::HashSet<K>`](https://doc.rust-lang.org/std/collections/struct.HashSet.html)       |
| `std::priority_queue<T>`  | [`std::collections::BinaryHeap<T>`](https://doc.rust-lang.org/std/collections/struct.BinaryHeap.html) |
| `std::span<T>`            | [`&[T]`](https://doc.rust-lang.org/std/primitive.slice.html)                                          |

For maps and sets instead of the container being parameterized over the hash or
comparison function used, the types require that the key types implement the
`std::hash::Hash` (unordered) or `std::cmp::Ord` (ordered) traits. To use the containers
with different hash or comparison functions, one must use a wrapper type with a
different implementation of the required trait.

Some C++ container types provided by the STL have no equivalent in Rust. Many of
those have equivalents available in third-party [libraries](../etc/libraries.md).

One significant different in the use of these types between C++ in Rust is with
the `Vec<T>` and array `[T; N]` types, from which slice references `&[T]` or
`&mut [T]` to part or all of the data can be cheaply created. For this reason,
when defining a function that does not modify the length of a vector and does
not need to statically know the number of elements in an array, it is more
idiomatic to take a parameter as `&[T]` or `&mut [T]` than as a reference to the
owned type.

In C++ it is better to take begin and end iterators or a range than a `span`
when possible, since iterators are more general. The same is true with Rust and
taking a generic type that implements `IntoIter<&T>` or `IntoIter<&mut T>`
instead of `&[T]`.

<div class="comparison">

```c++
#include <iterator>
#include <vector>

template <typename InputIter>
void go(InputIter first, InputIter last) {
  for (auto it = first; it != last; ++it) {
    // ...
  }
}

int main() {
  std::vector<int> v = {1, 2, 3};
  go(v.begin(), v.end());
}
```

```rust
use std::iter::IntoIterator;

fn go<'a>(iter: impl IntoIterator<Item = &'a mut i32>) {
    for x in iter {
        // ...
    }
}

fn main() {
    let mut v = vec![1, 2, 3];
    go(&mut v);
}
```

</div>

{{#quiz type_equivalents.toml}}
