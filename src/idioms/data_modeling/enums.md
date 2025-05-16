# Enums

In C++, enums are often used to model a fixed set of alternatives, especially when
each of those enumerators corresponds to a specific integer value, such as is needed
when working with hardware, system calls, or protocol implementations.

For example, the various modes for a GPIO pin could be modeled as an enum, which
would restrict methods using the mode to valid values.

While Rust enums are [more general](/idioms/data_modeling/tagged_unions.md),
they can still be used for this sort of modeling.

<div class="comparison">

```cpp
#include <cstdint>

enum Pin : uint8_t {
  Pin1 = 0x01,
  Pin2 = 0x02,
  Pin3 = 0x04
};

enum Mode : uint8_t {
  Output = 0x03,
  Pullup = 0x04,
  Analog = 0x27
  // ...
};

void low_level_set_pin(uint8_t pin, uint8_t mode);

void set_pin_mode(Pin pin, Mode mode) {
  low_level_set_pin(pin, mode);
}
```

```rust
#[repr(u8)]
#[derive(Clone, Copy)]
enum Pin {
    Pin1 = 0x01,
    Pin2 = 0x02,
    Pin3 = 0x04,
}

#[repr(u8)]
#[derive(Clone, Copy)]
enum Mode {
    Output = 0x03,
    Pullup = 0x04,
    Analog = 0x27,
    // ...
}

extern "C" {
    fn low_level_set_pin(pin: u8, mode: u8);
}

fn set_pin_mode(pin: Pin, mode: Mode) {
    unsafe {
        low_level_set_pin(pin as u8, mode as u8)
    };
}
```

</div>

The `#[repr(u8)]` attribute ensures that the representation of the enum is the
same as a byte (like declaring the underlying type of an enum in C++). The enum
values can then be freely converted to the underlying type with the `as`.

In C++ the standard way to convert from an integer to an enum is a static cast.
However, this [requires that the user check the validity of the cast
themselves](https://eel.is/c++draft/expr.static.cast#10). Often the conversion
is done by a function that checks that the value to convert is a valid enum
value.

In Rust the standard way to perform the conversion is to implement the `TryFrom`
trait for the type and then use the `try_from` method or `try_into` method.

<div class="comparison">

```cpp
$#include <cstdint>
$
$enum Pin : uint8_t {
$  Pin1 = 0x01,
$  Pin2 = 0x02,
$  Pin3 = 0x04
$};
$
struct InvalidPin {
    uint8_t pin;
};

Pin to_pin(uint8_t pin) {
  // The values are not contiguous, so we can't
  // just check the bounds and then cast.
  switch (pin) {
  case 0x1: { return Pin1; }
  case 0x2: { return Pin2; }
  case 0x4: { return Pin3; }
  }
  throw InvalidPin{pin};
}

int main() {
  try {
    Pin p(to_pin(2));
  } catch (InvalidPin &e) {
    return 0;
  }

  // use pin p
}
```

```rust
# #[repr(u8)]
# #[derive(Clone, Copy)]
# enum Pin {
#     Pin1 = 0x01,
#     Pin2 = 0x02,
#     Pin3 = 0x04,
# }
#
use std::convert::TryFrom;

struct InvalidPin(u8);

impl TryFrom<u8> for Pin {
    type Error = InvalidPin;

    fn try_from(
        value: u8,
    ) -> Result<Self, Self::Error> {
        match value {
            0x01 => Ok(Pin::Pin1),
            0x02 => Ok(Pin::Pin2),
            0x04 => Ok(Pin::Pin3),
            pin => Err(InvalidPin(pin)),
        }
    }
}

fn main() {
  let Ok(p) = Pin::try_from(2) else {
    return;
  };

  // use pin p
}
```

</div>

See [Exceptions and error handling](/idioms/exceptions.md) for examples of how
to ergonomically handle the result of `try_from`.

If low-level performance is more of a concern than memory safety,
`std::mem::transmute` is analogous to a C++ reinterpret cast, but requires
unsafe Rust because its use can result in undefined behavior. Uses of
`std::mem::transmute` for this purpose should not be hidden behind an interface
that can be called from safe Rust unless the interface can actually guarantee
that the call will never happen with an invalid value.

## Enums and methods

In C++ enums cannot have methods. Instead, to model an enum with methods one
must define a wrapper class for the enum and define the methods on that wrapper
class. In Rust, methods can be defined on an enum with an `impl` block, just
like any other type.

<div class="comparison">

```cpp
#include <cstdint>

// Actual enum
enum PinImpl : uint8_t {
  Pin1 = 0x01,
  Pin2 = 0x02,
  Pin3 = 0x04
};

class LastPin{};

// Wrapper type
struct Pin {
  PinImpl pin;

  // Conversion constructor so that PinImpl can be
  // used as a Pin.
  Pin(PinImpl p) : pin(p) {}

  // Conversion method so wrapper type can be
  // used with switch statement.
  operator PinImpl() {
    return this->pin;
  }

  Pin next() const {
    switch (pin) {
    case Pin1:
      return Pin(Pin2);
    case Pin2:
      return Pin(Pin3);
    default:
      throw LastPin{};
    }
  }
};
```

```rust
#[repr(u8)]
#[derive(Clone, Copy)]
enum Pin {
    Pin1 = 0x01,
    Pin2 = 0x02,
    Pin3 = 0x04,
}

struct LastPin;

impl Pin {
    fn next(&self) -> Result<Self, LastPin> {
        match self {
            Pin::Pin1 => Ok(Pin::Pin2),
            Pin::Pin2 => Ok(Pin::Pin3),
            Pin::Pin3 => Err(LastPin),
        }
    }
}
```

</div>

{{#quiz enums.toml}}
