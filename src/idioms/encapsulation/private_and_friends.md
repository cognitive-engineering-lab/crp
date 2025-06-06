# Private members and friends

## Private members

In C++ the unit of encapsulation is the class. Access specifiers (`private`,
`protected`, and `public`) that control access to members are enforced at the
class boundary.

In Rust the module is the unit of encapsulation. Item visibility (Rust's analog
to access specifiers) controls access to items at the module boundary.

<div class="comparison">

```cpp
#include <iostream>
#include <string>

class Person {
  int age;

public:
  std::string name;

  // Because age is private, a public constructor
  // method is needed to create instances.
  Person(std::string name, int age)
      : name(name), age(age) {}

  // Free functions cannot access private members,
  // so this has to be a member function.
  static void example() {
    Person alice{"Alice", 42};
    std::cout << alice.name << std::endl;
    // The private field is visible here, within
    // the class.
    std::cout << alice.age << std::endl;
  }
};

int main() {
  Person alice("Alice", 42);
  std::cout << alice.name << std::endl;
  // compilation error
  // std::cout << alice.age << std::endl;
}
```

```rust
mod person {
    pub struct Person {
        pub name: String,
        // this field is private
        age: i32,
    }

    impl Person {
        // Because age is private, a public
        // constructor method is needed to create
        // values outside of the person module.
        pub fn new(
            name: String,
            age: i32,
        ) -> Person {
            Person { name, age }
        }
    }

    // Free functions in the same module can
    // access private fields because the unit of
    // encapsulation is the module, not the
    // struct.
    fn example() {
        let alice =
            Person::new("Alice".to_string(), 42);
        println!("{}", alice.name);
        // The private field is visible here,
        // within the module.
        println!("{}", alice.age);
    }
}

use person::Person;

fn main() {
    let alice =
        Person::new("Alice".to_string(), 42);
    println!("{}", alice.name);
    // compilation error
    // println!("{}", alice.age);
}
```

</div>

In the Rust example, the [constructor for `Person` is
private](./private_constructors.md) because one of the
fields is private.

## Friends

Because encapsulation is at the module level in Rust, associated methods for
types can access internals of other types defined in the same module. This
subsumes most uses of the C++ `friend` declaration.

For example, defining a binary tree in C++ requires that the class representing
the nodes of the tree declare the main binary tree class as a friend in order
for it to access internal methods while keeping them private from other uses.
This would be required even if the `TreeNode` class were defined as an inner
class of `BinaryTree`.

In Rust, however, both types can be defined in the same module, and so have
access to each other's private fields and methods. The module as a whole
provides a collection of types, methods, and functions that together define a
encapsulated concept.

<div class="comparison">

```cpp
#include <memory>

class BinaryTree {
  // This needs to be an inner class in order for
  // it to be private.
  class TreeNode {
    friend class BinaryTree;

    int value;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;

  public:
    TreeNode(int value)
        : value(value), left(nullptr),
          right(nullptr) {}

  private:
    static void
    insert(std::unique_ptr<TreeNode> &node,
           int value) {
      if (node) {
        node->insert(value);
      } else {
        node = std::make_unique<TreeNode>(value);
      }
    }

    void insert(int value) {
      if (value < this->value) {
        insert(this->left, value);
      } else {
        insert(this->right, value);
      }
    }
  };

  std::unique_ptr<TreeNode> root;

public:
  BinaryTree() : root(nullptr) {}

  void insert(int value) {
    TreeNode::insert(root, value);
  }
};

int main() {
  BinaryTree b;
  b.insert(42);

  return 0;
}
```

```rust
mod binary_tree {
    pub struct BinaryTree {
        // This field is not visible outside of
        // the module.
        root: Option<Box<TreeNode>>,
    }

    impl BinaryTree {
        pub fn new() -> BinaryTree {
            BinaryTree { root: None }
        }

        pub fn insert(&mut self, value: i32) {
            insert(&mut self.root, value);
        }
    }

    // This struct and all its fields are not
    // visible outside of the module.
    struct TreeNode {
        value: i32,
        left: Option<Box<TreeNode>>,
        right: Option<Box<TreeNode>>,
    }

    impl TreeNode {
        fn new(value: i32) -> TreeNode {
            TreeNode {
                value,
                left: None,
                right: None,
            }
        }

        fn insert(&mut self, value: i32) {
            if value < self.value {
                insert(&mut self.left, value);
            } else {
                insert(&mut self.right, value);
            }
        }
    }

    // This free function is not visible outside
    // of the module.
    fn insert(
        node: &mut Option<Box<TreeNode>>,
        value: i32,
    ) {
        match node {
            None => {
                *node = Some(Box::new(
                    TreeNode::new(value),
                ));
            }
            Some(ref mut left) => {
                left.insert(value);
            }
        }
    }
}

// This brings the (public) type into scope.
use binary_tree::BinaryTree;

fn main() {
    let mut b = BinaryTree::new();
    b.insert(42);
}
```

</div>

## Passkey idiom

In the previous C++ example, the `TreeNode` constructor has to be public in
order to be used with `make_unique`. Fortunately, the constructor is still
inaccessible outside of the containing class, but it is not always the case that
such helper classes can be inner classes.

To make the constructor effectively private when it is not possible, one might
need to use a programming pattern like [the passkey
idiom](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/patterns/passkey.md).

The passkey idiom is also sometimes used to provide finer-grained control over
access to members than is possible with friend declarations. In either case, the
effect is achieved by modeling a capability-like system.

In Rust, it is possible to express the same idiom in order to achieve the same
effect.

<div class="comparison">

```cpp
#include <iostream>
#include <memory>
#include <string>

class Person {
  int age;

  class Passkey {};

public:
  std::string name;

  Person(Passkey, std::string name, int age)
      : name(name), age(age) {}

  static std::unique_ptr<Person>
  createPerson(std::string name, int age) {
    // Other uses of make_unique are not possible
    // because the Passkey type cannot be
    // constructed.
    return std::make_unique<Person>(Passkey(),
                                    name, age);
  }
};
```

```rust
pub trait Maker<K, B> {
    fn make(passkey: K, args: B) -> Self;
}

// Generic helper that we want to be able to call
// an otherwise private function or method.
fn alloc_thing<K, B, T: Maker<K, B>>(
    passkey: K,
    args: B,
) -> Box<T> {
    Box::new(Maker::<K, B>::make(passkey, args))
}

mod person {
    use super::*;
    use std::marker::PhantomData;

    pub struct Person {
        pub name: String,
        age: u32,
    }

    // A zero-sized type to act as the passkey.
    pub struct Passkey {
        // This field is zero-sized. It is also
        // private, which prevents construction
        // of Passkey outside of the person
        // module.
        _phantom: PhantomData<()>,
    }

    impl Person {
        // Private method that will be exposed
        // with a passkey wrapper.
        fn new(name: String, age: u32) -> Person {
            Person { name, age }
        }

        // Method that uses external helper that
        // requires access to another
        // otherwise-private method.
        fn alloc(
            name: String,
            age: u32,
        ) -> Box<Person> {
            alloc_thing(
                Passkey {
                    _phantom: PhantomData {},
                },
                MakePersonArgs { name, age },
            )
        }
    }

    // Helper structure needed to make the trait
    // providing the interface generic.
    pub struct MakePersonArgs {
        pub name: String,
        pub age: u32,
    }

    // Implementation of the trait that exposes
    // the method requiring a passkey.
    impl Maker<Passkey, MakePersonArgs> for Person {
        fn make(
            _passkey: Passkey,
            args: MakePersonArgs,
        ) -> Person {
            Person::new(args.name, args.age)
        }
    }
}
#
# fn main() {}
```

</div>

However the Passkey idiom is unlikely to be used in Rust because

- coupled types are usually defined in the same module (or a `pub (in path)`
  declaration can be used), making it unnecessary, and
- it requires cooperation from the interface by which the calling function will
  use a type.

The second point contrasts with the use above involving `std::make_unique` which
is able to forward to the underlying constructor without knowing about it at the
point of the definition of `std::make_unique`. While the example below is not
useful (because `alloc_thing` is not a useful helper), it does demonstrate what
would types have to be defined in order to achieve the same effect as when using
the idiom in C++.


## Friends and testing

Another common use of friend declarations is to make the internals of a class
available for unit testing. Though this practice is often discouraged in C++, it
is sometimes necessary in order to test other-wise private helper inner classes
or helper methods.

In Rust, tests are usually defined in the same module as the code being tested.
Because the content of modules is visible to submodules, this makes it so that
all of the content of the module is available for testing.

<div class="comparison">

```cpp
// Using Boost.Test
// https://www.boost.org/doc/libs/1_84_0/libs/test/doc/html/index.html
#include <string>

class Person {
public:
  std::string name;

private:
  int age;

  friend class PersonTest;

public:
  Person(std::string name, int age)
      : name(name), age(age) {}

  void have_birthday() {
    this->age = this->age + 1;
  }
};

#define BOOST_TEST_MODULE PersonTestModule
#include <boost/test/included/unit_test.hpp>

class PersonTest {
public:
  static void test_have_birthday() {
    Person alice("Alice", 42);
    BOOST_CHECK_EQUAL(alice.age, 42);

    alice.have_birthday();
    BOOST_CHECK_EQUAL(alice.age, 43);
  }
};

BOOST_AUTO_TEST_CASE(have_birthday_test) {
  PersonTest::test_have_birthday();
}
```

```rust
pub struct Person {
    pub name: String,
    age: u32,
}

impl Person {
    pub fn new(name: String, age: u32) -> Person {
        Person { name, age }
    }

    pub fn have_birthday(&mut self) {
        self.age = self.age + 1;
    }
}

#[cfg(test)]
mod test {
    use super::Person;

    #[test]
    fn test_have_birthday() {
        let mut alice =
            Person::new("alice".to_string(), 42);

        assert_eq!(alice.age, 42);
        alice.have_birthday();
        assert_eq!(alice.age, 43);
    }
}
```

</div>

<!-- Testing in Rust is described in more detail in the [chapter on unit
testing](/etc/unit_tests.md). -->

## Visibility of methods on Rust traits

Because traits in Rust are intended for the definition of interfaces, the
methods for some type that are declared by a trait are visible whenever both the
trait and the type are visible. In other words, it is not possible to have
private trait methods.

The default visibility for trait methods differs from Rust structs where the
default visibility is private to the defining module.

## Private constructors and friends

In C++ one can control which classes can derive from a specific class by making
all of the constructors private and then declaring classes which may derive from
it as friends.

In Rust, one can achieve the similar goal of controlling which types can
implement a trait by using the [sealed trait
pattern](https://predr.ag/blog/definitive-guide-to-sealed-traits-in-rust/).

{{#quiz private_and_friends.toml}}
