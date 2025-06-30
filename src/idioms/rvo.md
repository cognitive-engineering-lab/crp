# NRVO and RVO

<div class="warning">

Some of the statements about Rust in this chapter are dependent on the specifics
of how the compiler optimizes various programs. Unless otherwise state, the
results presented here are based on rustc 1.87 using the [2024 language
edition](https://doc.rust-lang.org/edition-guide/introduction.html) and with
`-O2` for C++ and `--opt-level=2` for Rust.

</div>

Unlike C++, Rust does not guarantee return value optimization (RVO). Neither
language guarantees named return value optimization (NRVO). However, RVO and
NRVO are usually applied in Rust where they would be in C++.

## RVO

The pattern where RVO and NRVO are likely most important is in static factory
methods (which Rust calls [constructor methods](./constructors.md)). In the
following example, C++17 and later guarantee RVO. In Rust the optimization is
performed reliably, but is not guaranteed.

<div class="comparison">

```cpp
struct Widget {
    signed char x;
    double y;
    long z;
};

Widget make(signed char x, double y, long z) {
    return Widget{x, y, z};
}
```

```rust
struct Widget {
    x: i8,
    y: f64,
    z: i64,
}

impl Widget {
    fn new(x: i8, y: f64, z: i64) -> Self {
        Widget { x, y, z }
    }
}
```

</div>

One can see in [the assembly][rvo-godbolt] that for both programs the value is
written directly into the destination provided by the caller.

<div class="comparison">

```asm
// C++
make(signed char, double, long):
        mov     BYTE PTR [rdi], sil
        mov     rax, rdi
        mov     QWORD PTR [rdi+16], rdx
        movsd   QWORD PTR [rdi+8], xmm0
        ret
```

```asm
// Rust
new:
        mov     rax, rdi
        mov     byte ptr [rdi + 16], sil
        movsd   qword ptr [rdi], xmm0
        mov     qword ptr [rdi + 8], rdx
        ret
```

</div>

[rvo-godbolt]: https://godbolt.org/#z:OYLghAFBqd5TKALEBjA9gEwKYFFMCWALugE4A0BIEAZgQDbYB2AhgLbYgDkAjF%2BTXRMiAZVQtGIHgBYBQogFUAztgAKAD24AGfgCsp5eiyahSAVyVFyKxqiIEh1ZpgDC6embZMQANnLOAGQImbAA5TwAjbFIpAFZyAAd0JWIHJjcPL19E5NShIJDwtiiYnnibbDs0kSIWUiIMz28/CqqhGrqiArDI6LjrWvrGrJbBruCe4r6ygEprdDNSVE4uS3M7AGoAdQJMYGwiDYBSAHYAISOtAEEN243NDYIADnJLm7uATxANmh9ZN7uGwAXt8CH9XtdTgARN5vAhsBL0ba7faHU4Xa6Ao4AJgAzEdYmczEwlCwaNgIEx0AB9NjGYCMGYEmGYu40JgbEIAdwgD2e5A2Xx%2B4OBoL%2BMw2AFojrjcMi9gdjucAYC7jsFWjlazVar1BD3jrAR99YbVUCTYboSrblbIScWVcuHN6NxYvxvFwdOR0NwAEoWQ5KBZLbDHPF8chEbROuYAaxAsWxADppD4TrEfLingBOHM8GS4wzcaT8NgJrTkD1en1cfhKEAVqOep3kOCwFAYHD4YhkSjUOiMVgcbgRwTCMQSTgyOTCZRqTTN8j6bGGeloHEXbFnazYWz2RwQZzDbwrwITIolEDZnIpffpdxNEArpK3tLdC99a%2BtO8dIYPrI8Due7VGM769KUAydMeBiWJ0YFTKUcxBosyzcGsZibOqqJKhiBobCkwAhJgGyoEgdT3DKuGApgCwRIwgqUdaGz0EIwDAoxdowriuFvNg6hENEHJYYqdKxhSBFESRZGkPcAo0WYdGhsazGscCErokxpAHIsQkogc6J6oKApAla3GwvaLYulwbqVtG3rcC4G4bvhwbLGGuLYvwTY6DMcxINgLA4DEEDOsWpblrZi41nWDaRtGvnkPGiYpmmGZZrm2b5tIhZWbi7p2dFcXNnMbYIPAEAdugCIMNEfYQBg1WMDEpA8E8JwVgOAmkPWEARHZETBHUXy8PwA2sKQHwAPIRLolRNhGDUcMIk1MPQw1ejgERmMALgSPQ9YjeQOB0iYkiLoQWlVAAbtgB1enxlRmAJdnBAJVlevQBARKQQ1uDgdlEKQ8IjvwN2kBEyTYFC2AnQywSgMVAhGMASgAGoENgXKTQkzAgzOojiJI05jooKgaHZ%2BiAUYJggOYliGF99aQHM6AJHeB2SpKLgbKzRCSowN30DKUKed6YNAzgTMhUBc1pE4TCuP%2B3g8KeCvwZefgvnk96ZMrz65He6t9C0u6y%2B0YzQSrMttEwv7jIU4HZLBf661IK7O/bkwa0hrlTqF1n5VF3AbHTRCoBsPBJm1SZaBsEDdiQMk4riPAzF58WtigCxEAkT11Q1CQ1aQoTsCsofh5H0f3YQie7AYJMTkTsgk3O5OLpT5Bcj9CQg/7NlVvwNaTU9ueHOgNAhwGFdRycMdx24jXRO5qfp8VcYgNIs%2BxB52U%2BDwWj708TyxC8VkluQZaxBWA/2bW1ixd5LalRVaBVYXTX52/RcgMAZSAZ10Qep9UXGNIaeNQETWmrNOweNFrMCICtNadlNrbV2vQfaeNjr0jOhtAgl17A3TuvwB6qAnorAjK9XcdlPrfV%2BlgFYXpAbA0OmDCGKhoaw0%2BjTRGNBkZowxljHGHpRzyEblOZu8hW4Li9MuVcNNUBOS3AzCIUsWZszSBzSwmBhYKK3BuPEUpJqi3QOLXYt14BIVNjbeWitXaATPA7BCBgtZ3ktjebWRsILfhAlBJWMErE/lAueR2gEPZuI9p4qQPsUKcGxH3QO1Zg7qCeD4SUfwNjAFQBXWISYeBxwTmQdycTV4%2BUziAbOo9P6L2LqXbgyTUnpMydk3JxCa5kDriuBuhNxH4ykRTJ8ndu69yLAHSKiSuDDxzk9HmE96lpOkBkrJEccl5IgAvd%2BS9k7FKKj5PyAUgrUH9ufS%2B18CrcBio2eK69N5Jm3tiXe%2B9D7H1PtwPKYzB7nJ2TGEZosb6FUfglMGKRHDSCAA%3D%3D

## NRVO

NRVO isn't guaranteed in either C++ or Rust, but the optimization often triggers
in cases where it is commonly desired. For example, in both C++ and Rust
when creating an array, initializing its contents, and then returning it, the
initialization assigns directly to the return location.

<div class="comparison">

```cpp
#include <array>

std::array<int, 10> make() {
    std::array<int, 10> v;
    for (int i = 0; i < 10; i++) {
        v[i] = i;
    }
    return v;
}
```

```rust
#[unsafe(no_mangle)]
fn new() -> [i32; 10] {
    let mut v = [0; 10];
    for i in 0..10 {
        v[i] = i as i32;
    }
    v
}
```

</div>

The [generated assembly][nrvo-godbolt] for the two versions of the program are
nearly identical, and both construct the array directly in the return location.

<div class="comparison">

```asm
// C++
make():
        movdqa  xmm0, XMMWORD PTR .LC0[rip]
        mov     rdx, QWORD PTR .LC2[rip]
        mov     rax, rdi
        movups  XMMWORD PTR [rdi], xmm0
        movdqa  xmm0, XMMWORD PTR .LC1[rip]
        mov     QWORD PTR [rdi+32], rdx
        movups  XMMWORD PTR [rdi+16], xmm0
        ret
.LC0:
        .long   0
        .long   1
        .long   2
        .long   3
.LC1:
        .long   4
        .long   5
        .long   6
        .long   7
.LC2:
        .long   8
        .long   9
```

```asm
// Rust
.LCPI0_0:
        .long   0
        .long   1
        .long   2
        .long   3
.LCPI0_1:
        .long   4
        .long   5
        .long   6
        .long   7
new:
        mov     rax, rdi
        movaps  xmm0, xmmword ptr [rip + .LCPI0_0]
        movups  xmmword ptr [rdi], xmm0
        movaps  xmm0, xmmword ptr [rip + .LCPI0_1]
        movups  xmmword ptr [rdi + 16], xmm0
        movabs  rcx, 38654705672
        mov     qword ptr [rdi + 32], rcx
        ret
```

</div>

[nrvo-godbolt]: https://godbolt.org/#z:OYLghAFBqd5TKALEBjA9gEwKYFFMCWALugE4A0BIEAZgQDbYB2AhgLbYgDkAjF%2BTXRMiAZVQtGIHgBYBQogFUAztgAKAD24AGfgCsp5eiyahSAVyVFyKxqiIEh1ZpgDC6embZMDzgDIEmbAA5TwAjbFIQAE5yAAd0JWIHJjcPLwN4xPshf0CQtnDImJtsO2SRIhZSIlTPbx5rbFtspgqqolzgsIjo60rq2vSGy3bO/MLogEprdDNSVE4uAFIAJgBmJYBWACEzJiUWGmwIJnQAfTZjYEZJrYARJa0AQRomAGpAgHcISbeAWiWa1wby22wIaxWgO2bx4WnuIIA7NtHk83mi3owiG82GYsQA3EFrO4gnZwtbQ2H3KEo9FvQSkN4ERnvLQAOlZsMRyOetNpeNBBHhgOJTJYSkZEOpPPRSwRD2laP5z1l8qeXGm9G4m343i4OnI6G4ACULFilLN5tgQes%2BOQiNp1dMANYgTYrVnSABsCM2nrWAA4ooGeDI1oZuNJ%2BGxXVpyLr9YauPwlCBY/a9eryHBYCgMDh8MQyJRqHRGKwONxbYJhGIJJwZHJhMo1JoM%2BR9CtDFc0KtkStto1msknExXO46iA1ptyH4Al0Cj1p5kkkJBt4p3EEiumGNupFpyUykI2gNx%2BkN4eWieOnPxou%2Bu015OD/0b3k967pua5gtuKs1gEqAeDghIuFUpAsAAnoCuAoiiliYCAIDgVBgIuAEVgwmSwKXE6xy/LK3KouiCFISh0FrOhwjkFhMFvPy5I0ui9JvBAGGMoSxJktCTJobR5KMr2vYEUiTG8vRApCkSgmMQqiKqrSpDYEQczvAxREqpmmpcNqcYOga3AuEJ/ZvN%2BlrWhC/DpjokzTEg2AsDgkQ/OGXCRuQ0abLG8b8ImyapnaDq2eQLpuh63q%2BgGQZRCG0hhtpaw6vpfmBRm0zZgg8AQLm6BsLEDARMWEAYHlBWRKQPD%2BgisalkQEQphAoT6aEARVJBlb8C1rCkJBADyoS6KU6a2iVHDCL1TD0O1bY4KEZjAGB9D0CmvD8DglwmJIM0EEpZR4tgK36tg6ilLiiz6hhTT6fQBChBBPVuDg%2BlEKQBDRqt5D7aQoQJNgdzYBt1wBKAaUCEYwBKAAagQ2CfL1sTMB1jaiOIkgNtWigqBo%2Bn6A0RgmCA5iWIYt0ppA0zoLELQrX8fwuG8lNEH8jD7fQworL5X2vTgZMuZew4QM4T6ejOo67gukQNsuLTC5uWTJOLEwNvzx6vrLKutK%2Bis9Mratnt4IsjNU2uS9IX4Wr%2BPAalqSVtombxE0QqAwqyVWslorEFiQDL/lbVlBVmKCzEQsS4kVJX5YwpBBOwiyO87PCuwi7v8NghDewQiENBjtZo7IGPNtjba4%2BQnwQbEHXWzptsJtwvW4qHWLoDQDumgnSfu6xbilVHFl%2B6lNnOiA0jJ5sEJxZ6sKwv6/qbP6rnuZ53nJdw/lpgHGXZWguWR4VVDFTvZUgMAPCbNnDB1aQDVNW2XVtUjd89f1g12Ejo3MEQE1Tfps3zYty1I3WlcLa%2BpCC7XsPtQ6qcTqoDOkjS62l9Q3Tum1R650rKvXeraL6P0VD/UBjdAmoMaDgyhjDOGCNdRVnkLnes%2Bd5CF1bPqDsXYCaoGMgOZBvMKZU2SDTBCwp2H9l7Osf4vUOYGi5pnA68AvxNCGgLIW%2BsQAIlFpgE2IARbS2SE%2BVR2ici3g/IbeRR5NaPmUaojW14NGGz1mkbwlitaGIlpo82P5OArCrrpHyBkuBvHUP6T0fxPTSDeMAVACdNgck9unMgFlPH%2B1BlvYOjdw6HyjjHCsXAAlBJCWEiJMIom2jToWbmIBOw51RnQ5GjCcblNLuXSurlvEry4PXEOuIGYtxycE0J4TInRIgN3XePt1gJIHo6cg9lHI9BctpReMY9J21XtYAK1lJkuhHqyMeKwJ5Ty0DPOerlEpLNrkmCZwVtISJ8SldZwUvqJEcNIIAA%3D%3D

## Determining whether the optimization occurred

Tools like [cargo-show-asm](https://github.com/pacak/cargo-show-asm) can be used
to show the assembly for individual functions in order to confirm that RVO or
NRVO was applied where desired.

There are also high-quality [benchmarking
tools](https://bheisler.github.io/criterion.rs/book/index.html) for Rust, which
can be used to ensure that changes do not unexpectedly result in worse
performance.
