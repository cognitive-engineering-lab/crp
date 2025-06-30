# Placement new

<div class="warning">

Some of the statements about Rust in this chapter are dependent on the specifics
of how the compiler optimizes various programs. Unless otherwise state, the
results presented here are based on rustc 1.87 using the [2024 language
edition](https://doc.rust-lang.org/edition-guide/introduction.html).

</div>

The primary purposes of placement new in C++ are

- situations where [storage allocation is separate from
  initialization](#custom-allocators-and-custom-containers) such as in the
  implementation of `std::vector` or memory pools,
- situations where the structures need to be placed at a specific memory
  location, e.g., for [working with memory-mapped
  registers](#memory-mapped-registers-and-embedded-development), and
- [storage reuse for performance reasons](#performance-and-storage-reuse).

You also might have ended up on this page looking for [how to construct large
values directly on the heap in Rust](#constructing-large-values-on-the-heap).

There is an [open proposal](https://github.com/rust-lang/rfcs/pull/2884) for
adding the features analogous to placement new in Rust, but the design of the
features is still under discussion. In the meantime, for many of the use cases
of placement new, there are either alternatives in safe Rust or approaches that
use unsafe Rust that can accomplish the required behaviors.

## Custom allocators and custom containers

It is uncommon to use placement new for the first reason because the major use
cases are covered by using STL containers with custom allocators. Similarly,
Rust's standard libraries can be used with custom allocators. However, in Rust
the API for custom allocators is still
[unstable](https://github.com/rust-lang/rust/issues/32838), and so they are only
available when using the nightly compiler with [a feature
flag](https://doc.rust-lang.org/unstable-book/library-features/allocator-api.html).
The Rust Book has [instructions on how to install the nightly
toolchain](https://doc.rust-lang.org/book/appendix-07-nightly-rust.html#unstable-features)
and the The Rust Unstable Book has [instructions on how to use unstable
features](https://doc.rust-lang.org/unstable-book/).

For stable Rust, there are libraries that cover many of the uses of allocators.
For example, [bumpalo](https://docs.rs/bumpalo/latest/bumpalo/) provides a safe
interface to a bump allocation arena, a [vector type using the
arena](https://docs.rs/bumpalo/latest/bumpalo/collections/vec/struct.Vec.html),
and other utility types using the arena.

For implementing custom collection types that involves separate allocation and
initialization of memory, the chapters in the Rustonomicon on [implementing
`Vec`](https://doc.rust-lang.org/nomicon/vec/vec.html) are a useful resource.

## Memory-mapped registers and embedded development

If you are using Rust for embedded development, you may want to additionally
read the [Embedded Rust Book](https://docs.rust-embedded.org/book/). The
chapters on
[peripherals](https://docs.rust-embedded.org/book/peripherals/index.html)
discuss how to work with structures that are located at a specific address in
memory.

The Embedded rust Book also includes [a chapter on advice for embedded C
programmers using Rust for embedded
development](https://docs.rust-embedded.org/book/c-tips/index.html).

## Performance and storage reuse

This use of placement new in C++ for the purpose of reusing storage can usually
be replaced in Rust by a simple assignment. Because [assignment in Rust is
always a move, and in Rust moves do not leave behind objects that require
destruction](./constructors/copy_and_move_constructors.md), the optimizer will
usually produce code analogous to placement new for this use case. In some
cases, this also depends on an [RVO or NRVO optimization](./rvo.md). While these
optimizations are not guaranteed, they are reliable enough for common coding
patterns, especially when combined with
[benchmarking](https://bheisler.github.io/criterion.rs/book/index.html) the
performance-sensitive code to confirm that the desired optimization was
performed. Additionally, the generated assembly for specific functions can be
examined using a tool like
[cargo-show-asm](https://github.com/pacak/cargo-show-asm).

The Rust version of the following example relies on the optimizations [to
achieve the desired behavior][godbolt-storage-reuse].

[godbolt-storage-reuse]: https://godbolt.org/#z:OYLghAFBqd5TKALEBjA9gEwKYFFMCWALugE4A0BIEAZgQDbYB2AhgLbYgDkAjF%2BTXRMiAZVQtGIHgBYBQogFUAztgAKAD24AGfgCsp5eiyahSAVyVFyKxqiIEh1ZpgDC6embZMpATnLOAGQImbAA5TwAjbFIQADZyAAd0JWIHJjcPL19E5NShIJDwtiiY%2BJtsOzSRIhZSIgzPbx4/csqhatqiArDI6LjrGrqGrOaBzu6ikriASmt0M1JUTi4AUgAmAGYVgFYAIRxSAgA3bAgAEWwaFjN6Immds5WtAEFLczsAagDa4GwAdQImF%2BRA%2BKwA7Lsns8PjCPoCQB8LAQAF7YchQ8GPF4YzY7XZmJhKFg0U5MdAAfTYxmAjHu2yxzxoTA%2BmApAHcyABrCBshHrWJsMwg76kX4AoHYO6giFQ2EfQSkOFw5laAB0qp4Wi00shLzlcoAVGzQRszl8fv9AcCdbL9Xb4XD0Xq7dLHhtddCXQB6L2IlQfNm2u2WTAgEBIYJEMMRIyoTnkiLodQQfls%2B7uoOujFghlQpkfKnBCDTG3OmGMEGCkFKVCkFhEVBIE1mkViq2SsM4K43IjFlYZsss9lclNrAVCj41usNpDpj2YqFcWb0bjbfjeLg6cjobgAJQs1fmi2woM2fHIUc3S9mnJA2zWquksTB21iGwAHD5PzwZBtDNxpH4Ng7y0cgNy3HcuH4JQQFAy8dFmOBYBQDAcHwYgyEoag6EYVgOG4c9BGEMQJE4GQ5GEZQ1E0K9yH0NZDGpNB1khNZdmsbBbHsRwIGcYZvDWHh/CYTAJl6GJZCSFJuPSdxGhAQScmktIxOKPpZFaGSOiGOSskUzSqkGLpgh6NSJLGHTMgEoTLHGEzJnU2YlCPJZuHWDZglQDwcBNFxUBDLt%2B1wHEPKYLyzB8/sXBCQMNmC7EXjeMxPlbS0JRBcEPRhEMwxSVFyRBQF%2B3nHNioxF4iGwNgEiMSrfKIABPBJmHYE8ABUgqhbB1Eq0hmSOdBAQ%2BGMWDjXYkwgNrT1idQ53K54BqG1k/hHRbMA%2BA0p3rRsS0y20FQ%2BCAcpAPLsAKpV%2BzNLRioujYXA%2BTVrvdOEWJY3aZUHa4SA2tkIBiw6tpnEtUvFYFMtVIbLpenM5sHH0/RPQNBxGsaJrTMrBzZABaIKAD8QfbXtYc9BdsVKhLnkjAsWCLd6so%2BCQCGAVglAgRnmfQGgIAJ9LphLRtakzGEODYMgGrxU7Oe5i1QclOk3XptafubAsqrFjHPWWkd0YHZ5SeeJd/y4NcwO0fhIJcV62MnFyT3ctZ%2BHg69yCQbAWAOahlwAoCQNN2jIOg2CLzNvnyFve9H2fV8Py/Hwf2kP8Vy4DZ1zN7duEdkPyCQhB4AgFD0GqhhoiwiAMCLxgYlIHh3zBUCcN6mCIAiNOImCWoGoI/g29YUgGoAeQiXQKkvc9y44YR%2B6YehO9onAIjMYAXAkegYN4fgcCpExJDnghSBH45sDXrduoqIVli3SNOLT%2BgCAiOs%2B7cHA06IQ5gPX8gTlIRMVAuLeaWCKAK8swaBGGAEoAAagQbAbJ%2B7NQ3IReQJFJDkSIooFQGg076CEkYEwIBzCWEMHfGCkBZjoASDJNeWMsb3XIUQLGjATj0Eug7bcX9Dg4BIcWDiXE0hOBEvxAwgR7LiQMFJPIskrJiNyDJVSUwbKcQPu0IygiFG8OUXZQooibIqN0k0CyxktFmSkE5W2JijYm3AubbgHwCENgeqqWuqptQQHQiQRU7keDTEzsA7OKB5hEASEKUu5cEjF1IKEVq3A7GoAcU4k%2BhB3HwiEmg5BZFZBoKopg2i2DyBsjrAkLuXtjap39twfuQogkgk5rYg8sSeCOLBM4w6bgK7RFPBsLxPiEI3hANIJp2wNhrATrETUmp3zvm2O%2BI2gFyDAW2KBKx6coLWCDk7RCiB85oELmEyuISdnhJAMAHg2wUkMEbtQFutEe4dy7uQG5fdB7DzsHc8ezAiBTxnmneei9l70FXncze1Id5bkIPvSoJxj78FPqgc%2Bdyr5Jy3Lfe%2BHcn4X0dm/O5X8f7YD/sCwBWdQEsHAVAmBcDmB3NSeIFBGT5BZJolueijE8GoCtuxZFXCyEULSFQkMl1WVsRYpsD4WN%2B6sPQOwwER94BOUUW0bwvEBF6KESJORfQhLiJkqopSEi1UxDUUopg2l6jKoNfK41eqDC2UsvJHRmjTLyNMQsVyaximWLTpBD46h3yxCxrEaQHxgCoHqdsDUh03FkA6a67phstkBKqfstpESolcG9b6/1gbg0PVDeebAiSyDwgYlS0iUhaWUQwQyvQCk8kFKKRY0pEFymVInDUtNfqA1BpDWGiArTdntPtt44OwDZiu3dn0bhSdZnzMWR6jOqy4Ih16f01UgzhkbFGVocZkzplJxTn7RtKz1nFNYUsgOQ6emf2iCkRw0ggA%3D%3D

<div class="comparison">

```cpp
#include <cstddef>
#include <new>

struct LargeWidget {
  std::size_t id;
};

template <typename T>
extern void blackBox(T &x);

void doWork(void *scratch) {
  for (std::size_t i = 0; i < 100; i++) {
    auto *w(new (scratch) LargeWidget{.id = i});
    // use w
    blackBox(w);
    w->~LargeWidget();
  }
}

int main() {
  alignas(alignof(LargeWidget)) char
      memory[sizeof(LargeWidget)];
  void *w = memory;
  doWork(w);
}
```

```rust
#[derive(Default)]
struct LargeWidget {
    id: usize,
}

fn do_work(w: &mut LargeWidget) {
    for i in 0..100 {
        *w = LargeWidget { id: i };
        // use w
        std::hint::black_box(&w);
    }
}

fn main() {
    let mut scratch = LargeWidget::default();
    do_work(&mut scratch);
}
```

</div>

Adding in a `Drop` implementation for `LargeWidget` does result in the drop
function being called on each loop iteration, but makes the generated assembly
much harder to read, and so has been omitted from the example.

## Constructing large values on the heap

`new` in C++ constructs objects directly in dynamic storage, and placement `new`
constructs them directly in the provided location. In Rust, `Box::new` is a
normal function, so the value is constructed on the stack and then moved to the
heap (or to the storage provided by the custom allocator).

While the initial construction of the value on the stack can sometimes be
optimized away, in order to guarantee that the stack is not used for the large
value requires the use of unsafe Rust and `MaybeUninit`. Additionally, the
mechanisms available for initializing a value on the heap do not guarantee that
the values will not be created on the stack and then moved to the heap. Instead,
they just make it possible to incrementally initialize a structure (either
field-by-field or element-by-element), so that the entire structure does not
have to be on the stack at once. The same optimizations do apply, however, and
so the additional copies might be avoided.

<div class="comparison">

```cpp
#include <iostream>
#include <memory>

int main() {
  constexpr unsigned int SIZE = 8000000;
  std::unique_ptr b = std::make_unique<
      std::array<unsigned int, SIZE>>();
  for (std::size_t i; i < SIZE; ++i) {
    (*b)[i] = 42;
  }

  // use b so that it isn't optimized away
  for (std::size_t i; i < SIZE; ++i) {
    std::cout << (*b)[i] << std::endl;
  }
}
```

<!-- must be no_run because mdbook test doesn't compile with opt-level=2 -->

```rust,no_run
fn main() {
    const SIZE: usize = 8_000_000;

    // optimization here makes it not overflow
    // the stack with opt-level=2
    let mut b = Box::new([0; SIZE]);
    for i in 0..SIZE {
        b[i] = 42;
    }

    // use b so that it isn't optimized away
    std::hint::black_box(&b);
}
```

</div>

On the other hand, directly defining the array as `[42; SIZE]` does result in
the value being first constructed on the stack, which produces an error when
run.

```rust,no_run
fn main() {
    const SIZE: usize = 8_000_000;

    let b = Box::new([42; SIZE]);

    // use b so that it isn't optimized away
    std::hint::black_box(&b);
}
```

```text
thread 'main' has overflowed its stack
fatal runtime error: stack overflow
Aborted (core dumped)
```

While construction of the values directly on the heap is not possible to
enforce, it is possible to incrementally construct the value by using unsafe
Rust, which avoids overflowing the stack. This technique relies on both
[`MaybeUninit`](https://doc.rust-lang.org/std/mem/union.MaybeUninit.html) and
[`addr_of_mut!`](https://doc.rust-lang.org/std/ptr/macro.addr_of_mut.html).

```rust,no_run
fn main() {
    const SIZE: usize = 8_000_000;
    let mut b = Box::<[i32; SIZE]>::new_uninit();
    let bptr = b.as_mut_ptr();
    for i in 0..SIZE {
        unsafe {
            std::ptr::addr_of_mut!(((*bptr)[i])).write(42);
        }
    }

    let b2 = unsafe { b.assume_init() };

    for i in 0..SIZE {
        println!("{}", b2[i]);
    }
}
```

Depending on what is need, this particular use can be generalized.

```rust
fn init_with<T, const SIZE: usize>(
    f: impl Fn(usize) -> T,
) -> Box<[T; SIZE]> {
    let mut b = Box::<[T; SIZE]>::new_uninit();
    let bptr = b.as_mut_ptr();
    for i in 0..SIZE {
        unsafe {
            std::ptr::addr_of_mut!(((*bptr)[i]))
                .write(f(i));
        }
    }

    unsafe { b.assume_init() }
}
```

Note that a more idiomatic way to deal with a large array on the heap is to
represent it as either a boxed slice or a vector instead of a boxed array, in
which case using iterators to define the value avoids constructing it on the
stack, and does not require the use of unsafe Rust.

```rust
fn init_with<T, const SIZE: usize>(
    f: impl Fn(usize) -> T,
) -> Box<[T]> {
    (0..SIZE).map(f).collect()
}
```
