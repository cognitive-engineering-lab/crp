# Inheritance and implementation reuse

Rust does not have inheritance and so the primary means of reuse of
implementations in Rust are composition, aggregation, and
[generics](/idioms/data_modeling/templates.md).

However, Rust traits do have support for default methods which resemble one
simple case of using inheritance for reuse of implementations. For example, in
the following example two virtual methods are used to support a method whose
implementation is provided by the abstract class.

<div class="comparison">

```cpp
#include <iostream>
#include <string>

class Device {
public:
    virtual void powerOn() = 0;
    virtual void powerOff() = 0;

    virtual void resetDevice() {
        std::cout << "Resetting device..." << std::endl;
        powerOff();
        powerOn();
    }

    virtual ~Device() {}
};

class Printer : public Device {
    bool powered = false;
public:
    void powerOn() override {
        this.powered = true;
        std::cout << "Printer is powered on." << std::endl;
    }

    void powerOff() override {
        this.powered = false;
        std::cout << "Printer is powered off." << std::endl;
    }
};

int main() {
    Printer myPrinter;
    myPrinter.resetDevice();
    return 0;
}
```

```rust
trait Device {
    fn power_on(&mut self);
    fn power_off(&mut self);

    fn reset_device(&mut self) {
        println!("Resetting device...");
        self.power_on();
        self.power_off();
    }
}

struct Printer {
    powered: bool,
}

impl Device for Printer {
    fn power_on(&mut self) {
        self.powered = true;
        println!("Printer is powered on");
    }

    fn power_off(&mut self) {
        self.powered = false;
        println!("Printer is powered off");
    }
}
```

</div>

In practice, the `resetDevice()` method in the `Device` class might be made
non-virtual in C++ if it is not expected that it will be overridden. In order to
make it align with the Rust example, we have made it virtual here, since Rust
traits can be used either for [dynamic
dispatch](/idioms/data_modeling/abstract_classes.md) or [static
dispatch](/idioms/data_modeling/concepts.md) (with [no vtable overhead in the
static dispatch
case](/idioms/data_modeling/abstract_classes.md#vtables-and-rust-trait-object-types)).

Rust traits differ from abstract classes in few more ways. For example,
Rust traits cannot define data members and cannot define private or protected
methods. This limits the effectiveness of using traits to implement the template
method pattern.

Rust traits also cannot be privately implemented. Anywhere that both a trait
and a type that implements that trait are visible, the methods of the trait are
visible as methods on the type.

Traits can, however, inherit from each other, including multiple inheritance. As
in modern C++, inheritance hierarchies in Rust tend to be shallow. In situations
with complex multiple inheritance, however, the diamond problem cannot arise in
Rust because traits cannot override other traits implementations. Therefore, all
paths to a common parent trait resolve to the same implementation.

{{#quiz inheritance_and_reuse.toml}}
