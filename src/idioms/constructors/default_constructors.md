# Default constructors

C++ has a special concept of default constructors to support several scenarios
in which they are implicitly called.
Rust does not have the same notion of a default constructor. The most similar mechanism is the [`Default`
trait](https://doc.rust-lang.org/std/default/trait.Default.html).

<div class="comparison">

```cpp
class Person {
    int age;

public:
    // Default constructor
    Person() : age(0) {}
}
```

```rust
struct Person {
   age: i32,
}

impl Person {
    pub const fn new() -> Self {
        Self { age: 0 }
    }
}

impl Default for Person {
    fn default() -> Self {
        Self::new()
    }
}
```

</div>


If a structure has a useful default value (such as would be constructed by a
default constructor in C++), then the type should provide
[both](https://rust-lang.github.io/api-guidelines/interoperability.html?highlight=default#types-eagerly-implement-common-traits-c-common-traits)
a `new` method that takes no arguments and an implementation of `Default`.

Because `Default` is a normal trait, the default constructor defined in the
example can be invoked using the usual syntax for invoking a static trait
method, e.g., `Default::default()`.

## Implicit initialization of class members

In C++, if a member is not explicitly initialized by a constructor, then it is
default-initialized. When the type of the member is a class, the
default-initialization invokes the default constructor.

In Rust, if all of the fields of a struct implement the `Default` trait, then an
implementation for the structure can be provided by the compiler.

<div class="comparison">

```cpp
class Person {
  int age;

public:
  Person() : age(0) {}
}

class Student {
  Person person;
}
```

```rust
#[derive(Default)]
struct Person {
    age: i32,
}

#[derive(Default)]
struct Student {
    person: Person,
}
```

</div>

The `#[derive(Default)]` macros in Rust are equivalent to writing the following.

```rust
struct Person {
    age: i32,
}

impl Default for Person {
    fn default() -> Self {
        Self {
            age: Default::default()
        }
    }
}

struct Student {
    person: Person,
}

impl Default for Student {
    fn default() -> Self {
        Self {
            person: Default::default()
        }
    }
}
```

Unlike C++ where the default initialization value for integers is indeterminate,
in Rust the default value for the primitive integer and floating point types [is
zero](https://doc.rust-lang.org/std/primitive.i32.html#impl-Default-for-i32).

<a name="struct-update"></a> Deriving the `Default` trait has a similar effect
on code concision as eliding initialization in C++. In situations where all of
the types implement the `Default` trait, but only some of the fields should have
their default values, one can use [struct update
syntax](https://doc.rust-lang.org/book/ch05-01-defining-structs.html#creating-instances-from-other-instances-with-struct-update-syntax)
to define a constructor method without enumerating the values for all of the
fields.

```rust
#[derive(Default)]
struct Person {
    age: i32,
}

#[derive(Default)]
struct Student {
    person: Person,
    favorite_color: Option<String>,
}

impl Student {
    pub fn with_favorite_color(color: String) -> Self {
        Student {
            favorite_color: Some(color),
            ..Default::default()
        }
    }
}
```

The performance of the struct update syntax <a
href="https://godbolt.org/#z:OYLghAFBqd5TKALEBjA9gEwKYFFMCWALugE4A0BIEAZgQDbYB2AhgLbYgDkAjF%2BTXRMiAZVQtGIHgBYBQogFUAztgAKAD24AGfgCsp5eiyahSAVyVFyKxqiIEh1ZpgDC6embZMQAJnLOAGQImbAA5TwAjbFIpAE5yAAd0JWIHJjcPL19E5NShIJDwtiiYnnibbDs0kSIWUiIMz28ea2xbeyEauqICsMjog0tuxqyWofreopKpAEprdDNSVE4uAFIfAGZVgFYAIRxSAgA3bAgAEWwaFjN6Ihmds9WtAEEEswiAaktzOw/VaKUQg%2BqwA7Lsns8PlCPixgJwPgQNn4IaDHi8UZsdvtosdThcrjc7g8IW9Pt8zL8amYcMJgWCIdCPgkAY4/iymOQGdCrkcyMRsAB9DD0MggD4AeQSHSYqw2LhqhxMstwnPRILRzwhBDYCXoHypNKIdPBL0ZpI%2BNCYHwA7sQkAKeXyiILhWQIK6YvqiIrgDMPgBaZX6to0Y1cxlQg3MI2gk2QiMJx2HZ1C9yi/XoDjutOkObhhPQgB0hfx11uIBAOAJtwg91NBdR%2BcbarOXDm9G42343i4OnI6G4ACULEbAYtlsDNnxyERtG25gBrEDbLSGbjSfhsZernt9gdcfhKECr2e9tvkOCwFAYHUMaKUag3hJ3mKkHgADhBq7otwB1Aic7kBEwR1AAntw07AawpCgeKES6JUp7TjeHDCOKTD0OBZ7kDgERmMALgSPQR68PwOBsMYwCSNhhCkIhuIkX22DqJUZjOhB/DBM6HbYfQBARKQYFuDggHetqHHkCcpARMk2AXBRJh8SYc5zDQRjAEoABqBDYNakrMBJgjCGIEicDIcjCMoaiaNh%2BgtEYynmJYhj8UekBzOgUppCR/ouB8nlEP6jAnPQspnD4/DoFJhw4G5tatO0aROEwrjuE0Uh%2BIEwR9MUAzvjkKTSiMzR%2BEkhVpJM/QxPlFRVJ0tT1MVGUJfR9XdJVuXVdYDUNGlox%2BOMPTZVMeVzGOSxme2nbdoB%2B4fE5RCoB8PCFp%2BhZaB8ED4MQZCThsPAzPwp46DMcxINgLAHNQi7bmuXAbuQu6Rdwh7HjOKlTVwEXkFu0jbOtWjSGU748NsPggz4IIAGyPbNL3vWecyXgg8AQNeWB4IQJAUFQtB3qwHCGfIJmSOZRmKCoGiAfofgOaYI4tXV3gQM4TUtFlhRVVILRlXk6R9c0BV8x10w8GMbStUwXSNQLgwS0z0tDZznXc91wyy2MPUiwMYtjQsE3cOsWx7AcuLnJcZZEtsGrmuSvz/KQgJWrG%2BawvCiLIi2KLopiJs4ic5vVlbNvvF83oUkaUa0i79ZQsyjusg7TuqvGUJJvyqYip6krSrK8piUqGwqii6re882q6l61LRmGsdMqHlo2naDosLyyYujm2ZZ2KCrBL6AZBiIIa16nEaMEabBsV8wIbGcwb0DQFZVpbtaynGBZfIW6cph6M9zyImanB69wbOvBZKE2pctuePFdrD2H7sOlhfPrE5G99x3zuQ52XQM8VLiuO6D0twbBBIWbY989zwyPCeFSF5EBozQJmZ8jAcaPmQS%2BEAb5PzfgYM6R2/5AJQTAhJYhME4IITsBJFC0Z0KYUArhfChF6DEQkuRSi1E%2By0XoicRi/BmKsXYqRSgwg2iAT4gJISGNRKHC3MIqSMkVDyQ4X3OBalYRaR0npZkPZpzkxJmZWQ5MrJU1stkOmWCGYSLih5LyQgfJ%2BQCkFbAIUwrfSitEGK2AbGM2lMlVKmRBYcxyqLIWRUNZhIqsNLm4tEptRloEuWcSpZa2iSrTW6tEkZImGk0WetxycB8J9O%2BT1%2BzcHmiOJaK01obS2ljXa79DoIxOmdC6V1/63R4g9Up%2B5XqwMRp9b6W5tjSELLEWI2x3xaA/NIEEH4QQ%2BD%2BpA56B5mlfyXDwLQq4eIbBmg/eGn9TqSQBElaQQA%3D%3D%3D">is
up to the optimizer</a>.

## Implicit initialization of array values

In C++, arrays without explicit initialization are default-initialized using the
default constructors.

In Rust, the value with which to initialize the array must be provided.

<div class="comparison">

```cpp
class Person {
  int age;

public:
  Person() : age(0) {}
};

int main() {
  Person people[3];
  // ...
}
```

```rust
#[derive(Default)]
struct Person {
    age: i32,
}

fn main() {
    // std::array::from_fn provides the index to the callback
    let people: [Person; 3] =
        std::array::from_fn(|_| Default::default());
    // ...
}
```

</div>

If the type happens to be [trivially
copyable](./copy_and_move_constructors.md#trivially-copyable-types),
then a shorthand can be used.

```rust
#[derive(Clone, Copy, Default)]
struct Person {
    age: i32,
}

fn main() {
    let people: [Person; 3] = [Default::default(); 3];
    // ...
}
```

## Container element initialization

In C++, the default constructor could be used to implicitly define collection
types, such as `std::vector`. Before C++11, one value would be default
constructed, and the elements would be copy constructed from that initial
element. Since C++11, all elements are default constructed.

As with array initialization, the values must be explicitly specified in Rust.
The vector can be constructed from an array, enabling the same syntax as with
arrays.

<div class="comparison">

```cpp
#include <vector>

class Person {
    int age;

public:
    Person() : age(0) {}
}

int main() {
    std::vector<Person> people(3);
    // ...
}
```

```rust
#[derive(Default)]
struct Person {
    age: i32,
}

fn main() {
    let people_arr: [Person; 3] =
        std::array::from_fn(|_| Default::default());
    let people: Vec<Person> = Vec::from(people_arr);
    // ...
}
```

</div>

In Rust, the vector can also be constructed [from an
iterator](https://doc.rust-lang.org/book/ch13-02-iterators.html#methods-that-produce-other-iterators).

```rust
#[derive(Default)]
struct Person {
    age: i32,
}

fn main() {
    let people: Vec<Person> = (0..3).map(|_| Default::default()).collect();
    // ...
}
```

If the type implements the `Clone` trait, then the array can be constructed
using the `vec!` macro. See the chapter on [copy
constructors](./copy_and_move_constructors.md) for more
details on `Clone`.

```rust
#[derive(Clone, Default)]
struct Person {
    age: i32,
}

fn main() {
    let people: Vec<Person> = vec![Default::default(); 3];
    // ...
}
```

## Implicit initialization of local variables

In C++, the default constructor is used to perform default-initialization of
local variables that are not explicitly initialized.


In Rust, initialization of local variables is always explicit.

<div class="comparison">

```cpp
class Person {
    int age;

public:
    Person() : age(0) {}
};

int main() {
    Person person;
    // ...
}
```

```rust
#[derive(Clone, Default)]
struct Person {
    age: i32,
}

fn main() {
    let person = Person::default();
    // ...
}
```

</div>

## Implicit initialization of the base class object

In C++, the default constructor is used to initialize the base class object if
no other constructor is specified.

```cpp
class Base {
  int x;

public:
  Base() : x(0) {}
};

class Derived : Base {
public:
  // Calls the default constructor for Base
  Derived() {}
};
```

Since Rust does not have inheritance, there is no equivalent to this case.
See the chapter on [implementation
reuse](../data_modeling/inheritance_and_reuse.md) or the section on [traits
in the Rust book](https://doc.rust-lang.org/book/ch10-02-traits.html) for
alternatives.

## `std::unique_ptr`

There are some additional cases where the `Default` trait is used in Rust, but
default constructors are not used for initialization in C++.

Rust's equivalent of smart pointers implement `Default` by delegating to the
`Default` implementation of the contained type.

```rust
#[derive(Default)]
struct Person {
    age: i32,
}

fn main() {
    let b: Box<Person> = Default::default();
    // ...
}
```

This differs from the treatment of `std::unique_ptr` in C++ because unlike `Box`,
`std::unique_ptr` is nullable, and so the default constructor for
`std:unique_ptr` produces a pointer that owns nothing. The equivalent type in
Rust is `Option<Box<Person>>`, for which the `Default` implementation produces
`None`.

## Other uses of `Default`

[`Option::unwrap_or_default`](https://doc.rust-lang.org/std/option/enum.Option.html#method.unwrap_or_default)
makes use of `Default`, which makes getting a default value when the `Option`
does not contain a value more convenient.

<div class="comparison">

```cpp
#include <optional>
#include <string>

void go(std::optional<std::string> x) {
  std::string a =
      x.or_else([]() {
         return std::make_optional<std::string>();
       }).value();
  // if x was nullopt, then a is ""

  // ...
}
```

```rust
fn go(x: Option<String>) {
    let a: String = x.unwrap_or_default();
    // if x was None, then a is ""

    // ...
}
```

</div>

In C++ `std::optional::value_or` is not equivalent, because it would always
construct the default, as opposed to only constructing the default when the
`std::optional` is `std::nullopt`. The equivalent requires using
`std::optional::or_else`.

{{#quiz default_constructors.toml}}
