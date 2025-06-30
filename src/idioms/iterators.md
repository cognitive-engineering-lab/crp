# Iterators and ranges

Rust iterators resemble C++
[ranges](https://en.cppreference.com/w/cpp/ranges.html) in that they represent
iterable sequence and can be manipulated similarly to using
range views. Since C++ ranges are defined using iterators and ranges were only
introduced in C++20, this chapter compares Rust iterators with both C++
iterators and with C++ ranges.

Rust iterators are forward iterators, not [bidirectional or random
access](#bidirectional-and-random-access-iterators). The definition of the
`Iterator` trait reflects this: all of its methods are based on a `next` method
which returns either an `Option::Some` containing the next item in the iteration
or `Option::None`.

A Rust iterator also does not represent an index into a structure the way that a
C++ iterator does when used with functions from the C++ STL algorithms library,
such as `std::sort`.

Whether Rust iterators are input iterators or input/output iterators depends on
whether the iterated items are owned values (input), references (input), or
mutable references (input/output). The type of the iterated values typically
reflects whether the structure being iterated over is an owned value, reference,
or mutable reference. Rust iterators cannot be output iterators only because the
iterated values must always be initialized.

In a sense, Rust's iterators are much more like C++23 generators (except that
Rust [does not yet support
coroutines](https://github.com/rust-lang/rust/issues/43122)).

## Iterators, ranges, and `for` loops

In C++, anything that has `begin()` and `end()` methods to return iterators
(i.e., anything that models the C++20 `range` concept) can be used with a for
loop. In Rust, anything that implements the
[`IntoIterator`](https://doc.rust-lang.org/std/iter/trait.IntoIterator.html)
trait can be used with a for loop. This includes iterators themselves, which
implement the trait via a [blanket
implementation](https://doc.rust-lang.org/std/iter/trait.IntoIterator.html).

<div class="comparison">

```cpp
#include <iostream>
#include <vector>

int main() {
  std::vector<int> v{1, 2, 3};

  // prints 1, 2, 3
  for (auto &x : v) {
    std::cout << x << std::endl;
    x = x + 1;
  }

  // prints 2, 3, 4
  for (const auto &x : v) {
    std::cout << x << std::endl;
  }
}
```

```rust
fn main() {
    let mut v = vec![1, 2, 3];

    // prints 1, 2, 3
    for x in &mut v {
        println!("{}", x);
        *x = *x + 1;
    }

    // prints 2, 3, 4
    for x in &v {
        println!("{}", x);
    }
}
```

</div>

In both C++ and Rust, iterators can be used for reading, writing, or both. In
Rust, the use of an iterator for writing depends on the type of the elements
returned. In the case of `Vec<i32>` above, the `IntoIterator` trait implemented for
`&mut Vec<i32>` produces an iterator over mutable references `&mut i32`, which
enables modifying the values in the vector.[^mut-iterator-safety]

[^mut-iterator-safety]: The safety of the mutable references is given by the
    fact that the references borrow from the vector, don't overlap, and are
    never produced by the iterator more than once.

## Ranges and views

Just as the [C++ ranges library](https://en.cppreference.com/w/cpp/ranges.html)
provides many utility functions for defining pipelines to transform ranges, the
Rust standard library defines many [iterator
methods](https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.filter)
transforming Rust iterators, including turning them back into collections like
vectors.

<div class="comparison">

```cpp
#include <ranges>
#include <vector>

using namespace std::views;
using namespace std::ranges::views;

int main() {
$  // clang-format off
  // This example requires C++23
  auto v =
    iota(1)
      | filter([](int n) { return n % 2 == 1; })
      | transform([](int n) { return n + 3; })
      | take(10)
      | std::ranges::to<std::vector>();
$  // clang-format on

  // use v...
}
```

```rust
fn main() {
    let v = (1..)
        .filter(|i| i % 2 == 1)
        .map(|i| i + 3)
        .take(10)
        .collect::<Vec<i32>>();

    // use v...
}
```

</div>

The Rust `collect` iterator method can convert the iterator into anything that
implements `FromIterator`. If the type of `v` can be inferred from its later
use, the type does not need to be specified in the call to `collect`.

In both C++ and in Rust, the view or iterator could be used directly as the
value to loop over, without first converting to something like
a vector. Similarly, in both languages the construction of the values is done
lazily.

<div class="comparison">

```cpp
#include <ranges>
#include <iostream>

using namespace std::views;
using namespace std::ranges::views;

int main() {
$  // clang-format off
    for (auto x :
        iota(1)
          | filter([](int n) { return n % 2 == 1; })
          | transform([](int n) { return n + 3; })
          | take(10)) {
      std::cout<< x << std::endl;
    }
$  // clang-format on
}
```

```rust
fn main() {
    for x in (1..)
        .filter(|i| i % 2 == 1)
        .map(|i| i + 3)
        .take(10)
    {
        println!("{}", x);
    }
}
```

</div>

Additional useful methods on iterators are provided by the third-party
[itertools crate](https://docs.rs/itertools/latest/itertools/)
via [extension traits](../patterns/adapter.md#extension-traits).

## `IntoIterator` and ownership

The [`IntoIterator`
trait](https://doc.rust-lang.org/std/iter/trait.IntoIterator.html) can be
implemented for a type `T` itself, a reference `&T`, or a mutable reference
`&mut T`. The possible types of the iterated items depend on what type the trait
is implemented for. If it is implemented for `&mut T`, for example, typically
the items will be mutable references to items still owned by the original
structure. If the type is `T`, then the items will be the owned items that were
in the original structure.[^owned-items-references]

[^owned-items-references]: The owned items might themselves be references,
    however. E.g., calling `into_iter` on a `Vec<&str>` will not result in
    iterating over `String` values even though the vector itself is consumed.

Since the behavior depends on the type of the structure used in the for loop,
which may be inferred and therefore not visible, this can lead to surprising
compilation errors. In particular, iterating over a vector `v` instead of a
reference to the vector `&v` will consume the original vector, making it
inaccessible.

Iterating over a structure in C++ is most similar to calling `into_iter` on a
mutable reference in Rust.

```rust
fn main() {
    let mut v = vec![
        String::from("a"),
        String::from("b"),
        String::from("c"),
    ];

    for x in &v {
        // x: &String
        println!("{}", x);
    }

    // since v was borrowed, not moved, it is still accessiable here.
    println!("{:?}", v);

    for x in &mut v {
        // x: &mut String
        x.push('!');
    }

    // since v was borrowed, not moved, it is still accessiable here.
    // however, the content of v has been modified
    println!("{:?}", v);

    for x in v {
        // x: String
        // drops x at the end of each iteration
    }

    // v is no longer accessible, so this wouldn't compile
    // println!("{:?}", v);
}
```

Most iterable types will also provide methods specifically for accessing the
reference or mutable reference iterators. Conventionally, these are called
`iter` and `iter_mut`. They are useful in situations where the iteration is not
being immediately used with a for loop, but instead is used with other iterator
methods, because of the relative precedence of the reference operator and of
method invocation.

## Identifying ranges for algorithms to operate on

C++ uses iterators to identify regions of structures on which functions from the
STL algorithms library should operate. Rust iterators do not serve that purpose.
Instead, there are two common alternatives.

The first is that operations which operate strictly on forward iterators just
operate directly on the iterator. Identifying specific parts of an iterator for
this purpose can be done using the [iterator
methods](https://doc.rust-lang.org/std/iter/trait.Iterator.html#provided-methods),
such as
[`take`](https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.take) or
[`filter`](https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.filter).
Alternatively, for some types [slices can be taken by indexing with a
range](https://doc.rust-lang.org/book/ch04-03-slices.html) before converting to
an iterator.

<div class="comparison">

```cpp
#include <algorithm>
#include <vector>

int main() {
  std::vector v{1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto begin = v.begin() + 2;
  auto end = begin + 5;
  bool b(std::any_of(begin, end, [](int n) {
    return n % 2 == 0;
  }));
}
```

```rust
fn main() {
    let v: Vec<i32> = (1..10).collect();
    let b = v
        .iter()
        .skip(2)
        .take(5)
        .any(|n| n % 2 == 0);

    // or

    let b2 = v[3..7].iter().any(|n| n % 2 == 0);

    // ...
}
```

</div>

The second is that some of the algorithms operate on slices. For example, the
[sort method](https://doc.rust-lang.org/std/primitive.slice.html#method.sort) in
the Rust standard library operates on slices. This is similar to if in C++, a
function were to operate on a `std::span` instead of on an iterator. Many of the
methods available on `Vec<T>` are actually defined on `&[T]` and are made
available on `Vec<T>` via [deref
coercion](https://doc.rust-lang.org/book/ch15-02-deref.html).

<div class="comparison">

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
  std::vector v{9, 8, 7, 6, 5, 4, 3, 2, 1};

  for (auto n : v) {
    std::cout << n << ",";
  }
  std::cout << std::endl;

  std::sort(v.begin(), v.end());

  for (auto n : v) {
    std::cout << n << ",";
  }
  std::cout << std::endl;
}
```

```rust
fn main() {
    let mut v: Vec<i32> = (1..10).rev().collect();
    println!("{:?}", v);

    v.sort();

    println!("{:?}", v);
}
```

</div>

## Iterator invalidation

In C++, operations sometimes only invalidate some iterators on a value, such as
the `erase` method on `std::vector` only invaliding iterators to the erased
element and those after it, but not the ones before it.

In Rust, the fact that iterators borrow the whole iterated value means that no
operations modifying the value itself (such as erasing values from a vector) can
be performed while iterating. Thus, there are no iterator invalidation rules to
keep in mind while using Rust iterators.

However, this also means that there are things that can be done with iterators
in C++ that cannot be done with iterators in Rust, such as removing elements
from a vector while iterating over it. Instead in Rust there are two possible
approaches: use indices or use helper methods.

Using indices instead of iterators comes with the same challenges as it does in
C++, with the exception that the program will panic instead of performing
undefined behavior if an index is out of bounds in safe Rust.

Using the helper methods resembles the recommendations commonly given for
writing against newer C++ standards. For example, to remove all elements of a
particular value in Rust, one would use the
[`Vec::retain`](https://doc.rust-lang.org/std/vec/struct.Vec.html#method.retain)
method, which is like `remove_if` or `erase_if` on `std::vector`, but with a
negative predicate.

<div class="comparison">

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
  std::vector<int> v{1, 2, 3};

  auto newEnd = remove(v.begin(), v.end(), 2);
  v.erase(newEnd, v.end());

  // Or since C++20
  // std::erase(v, 2);

  for (auto x : v) {
    std::cout << x << std::endl;
  }
}
```

```rust
fn main() {
    let mut v = vec![1, 2, 3];
    v.retain(|i| *i != 2);

    for x in &v {
        println!("{}", x);
    }
}
```

</div>

When using iterators, one would use the methods described in the section on
[ranges and views](#ranges-and-views).

## Implementing Rust iterators

This extended example defines a binary tree and preorder const iterator over the
tree. The module structure is included in this example because defining private
items will be an important part of later patterns used to simplify the
implementation.

<div class="comparison">

```cpp
#include <memory>

template <typename V>
class Tree {
public:
    V value;
    std::unique_ptr<Tree<V>> left;
    std::unique_ptr<Tree<V>> right;
};
```

```rust
mod tree {
    /// Binary tree with values at every node.
    /// Not necessarily balanced.
    pub struct Tree<V> {
        pub value: V,
        pub left: Option<Box<Tree<V>>>,
        pub right: Option<Box<Tree<V>>>,
    }
}
```

</div>

Much like how in C++ an iterator and a const iterator are distinct things, in
Rust there are different iterators for owned values, references, and mutable
references.

For example, for tree type `Tree<V>` would likely provide the following methods
to support preorder iteration. For the methods that provide iteration over
references, the references are borrowed from the original structure, and so the
lifetime parameter `'a` relates the reference to the item to `self`.

|                   | method                                                        | item type   |
|-------------------|---------------------------------------------------------------|-------------|
| reference         | `fn preorder<'a>(&'a self) -> IterPreorder<'a, V>`            | `&'a V`     |
| mutable reference | `fn preorder_mut<'a>(&'a mut self) -> IterMutPreorder<'a, V>` | `&'a mut V` |
| owned             | `fn into_preorder(self) -> IntoIterPreorder<V>`               | `V`         |

Just like with C++ iterators, the essential complexity of defining the iterator
amounts to determining how to capture the suspended state of traversing the
structure. In this case the suspended state consists of a stack of the rest of
the trees to iterate.

The implementations diverge in the interface provided. C++ requires several
types and methods to be defined in order for the type to model a forward
iterator. Rust requires only the definition of the element type that will be
iterated and a `next` method.

<div class="comparison">

```cpp
$#include <memory>
$#include <vector>
$
template <typename V>
class Tree {
public:
  class iterator {
    std::vector<const Tree<V> *> rest;

  public:
    using difference_type = long;
    using value_type = V;
    using pointer = const V *;
    using reference = const V &;
    using iterator_category =
        std::forward_iterator_tag;

    iterator() {}
    iterator(const Tree<V> *start) {
      rest.push_back(start);
    }

    reference operator*() const {
      return rest.back()->value;
    }

    iterator &operator++() {
      const Tree<V> *t = rest.back();
      rest.pop_back();
      if (t->right) {
        rest.push_back(t->right.get());
      }
      if (t->left) {
        rest.push_back(t->left.get());
      }
      return *this;
    }

    iterator operator++(int) {
      iterator retval = *this;
      const Tree<V> *t = rest.back();
      rest.pop_back();
      if (t->right) {
        rest.push_back(t->right.get());
      }
      if (t->left) {
        rest.push_back(t->left.get());
      }
      return retval;
    }

    bool operator==(const iterator &other) const {
      return rest == other.rest;
    }

    bool operator!=(const iterator &other) const {
      return !(*this == other);
    }
  };
$
$  V value;
$  std::unique_ptr<Tree<V>> left;
$  std::unique_ptr<Tree<V>> right;
};
```

```rust
mod tree {
#     /// Binary tree with values at every node.
#     /// Not necessarily balanced.
#     pub struct Tree<V> {
#         pub value: V,
#         pub left: Option<Box<Tree<V>>>,
#         pub right: Option<Box<Tree<V>>>,
#     }
#
    pub struct IterPreorder<'a, V>(Vec<&'a Tree<V>>);

    impl<'a, V> Iterator for IterPreorder<'a, V> {
        type Item = &'a V;

        // This is like a combination of
        // operator++ and operator*
        fn next(&mut self) -> Option<&'a V> {
            let Tree { value, left, right } = self.0.pop()?;
            if let Some(right) = right {
                self.0.push(right.as_ref());
            }
            if let Some(left) = left {
                self.0.push(left.as_ref());
            }
            Some(value)
        }
    }
}
```

</div>

The remaining step is to make the original type iterable. In C++, this involves
defining `begin` and `end` methods. In Rust, this involves either implementing a
method that explicitly produces the iterator, or implementing the `IntoIterator`
trait.

When there are multiple possible iterations for a type and none of them are
canonical, it is idiomatic to omit the `IntoIterator` trait implementation.
Omitting the implementation requires users to intentionally choose the kind of
iteration to use. The trait is implemented below to provide an example, but an
unsorted binary tree is a case where it would be typical to omit the trait
implementation, to force a user to pick between pre-, post-, and in-order
iteration.

Because the implemented iterator is one for references, the trait actually is
implemented for references to trees `&Tree<V>`, rather than `Tree<V>` directly.

<div class="comparison">

```cpp
#include <iostream>
$#include <memory>
$#include <vector>

template <typename V>
class Tree {
public:
$  class iterator {
$    std::vector<const Tree<V> *> rest;
$
$  public:
$    using difference_type = long;
$    using value_type = V;
$    using pointer = const V *;
$    using reference = const V &;
$    using iterator_category =
$        std::forward_iterator_tag;
$
$    iterator() {}
$    iterator(const Tree<V> *start) {
$      rest.push_back(start);
$    }
$
$    reference operator*() const {
$      return rest.back()->value;
$    }
$
$    iterator &operator++() {
$      const Tree<V> *t = rest.back();
$      rest.pop_back();
$      if (t->right) {
$        rest.push_back(t->right.get());
$      }
$      if (t->left) {
$        rest.push_back(t->left.get());
$      }
$      return *this;
$    }
$
$    iterator operator++(int) {
$      iterator retval = *this;
$      const Tree<V> *t = rest.back();
$      rest.pop_back();
$      if (t->right) {
$        rest.push_back(t->right.get());
$      }
$      if (t->left) {
$        rest.push_back(t->left.get());
$      }
$      return retval;
$    }
$
$    bool operator==(const iterator &other) const {
$      return rest == other.rest;
$    }
$
$    bool operator!=(const iterator &other) const {
$      return !(*this == other);
$    }
$  };
$
  iterator begin() const {
    return iterator(this);
  }

  iterator end() const {
    return iterator();
  }
$
$  V value;
$  std::unique_ptr<Tree<V>> left;
$  std::unique_ptr<Tree<V>> right;
};

int main() {
  Tree<int> t{1,
              std::make_unique<Tree<int>>(
                  2, nullptr, nullptr),
              std::make_unique<Tree<int>>(
                  3,
                  std::make_unique<Tree<int>>(
                      4, nullptr, nullptr),
                  nullptr)};

  for (auto v : t) {
    std::cout << v << std::endl;
  }
}
```

```rust
mod tree {
#    /// Binary tree with values at every node.
#    /// Not necessarily balanced.
#    pub struct Tree<V> {
#        pub value: V,
#        pub left: Option<Box<Tree<V>>>,
#        pub right: Option<Box<Tree<V>>>,
#    }
#
#    pub struct IterPreorder<'a, V>(Vec<&'a Tree<V>>);
#
#    impl<'a, V> Iterator for IterPreorder<'a, V> {
#        type Item = &'a V;
#        fn next(&mut self) -> Option<&'a V> {
#            match self.0.pop() {
#                None => None,
#                Some(t) => {
#                    let Tree { value, left, right } = t;
#                    if let Some(right) = right {
#                        self.0.push(right.as_ref());
#                    }
#                    if let Some(left) = left {
#                        self.0.push(left.as_ref());
#                    }
#                    Some(value)
#                }
#            }
#        }
#    }
#
    impl<V> Tree<V> {
        pub fn preorder(&self) -> IterPreorder<V> {
            IterPreorder(vec![self])
        }
    }

    impl<'a, V> IntoIterator for &'a Tree<V> {
        type Item = &'a V;
        type IntoIter = IterPreorder<'a, V>;

        fn into_iter(self) -> Self::IntoIter {
            self.preorder()
        }
    }
}

use tree::*;

fn main() {
    let t = Tree {
        value: 1,
        left: Some(Box::new(Tree {
            value: 2,
            left: None,
            right: None,
        })),
        right: Some(Box::new(Tree {
            value: 3,
            left: Some(Box::new(Tree {
                value: 4,
                left: None,
                right: None,
            })),
            right: None,
        })),
    };

    for n in t.preorder() {
        println!("{}", n);
    }

    for n in &t {
        println!("{}", n);
    }
}
```

</div>

Implementing the iterators for mutable references and owned values can be done
similarly. With all three, there are three `IntoIterator` implementations, one
for `&Tree<V>`, one for `&mut Tree<V>`, and one for `Tree<V>`.

### Reducing code duplication

As with implementing iterators and const iterators in C++, implementing
iterators for owned values, references, and mutable references can result in
significant code duplication in Rust.

One pattern for addressing this involves defining a private trait that captures
the decomposing of the type and then implementing the `Iterator` trait via a
blanket implementation in terms of that trait. Wrapper structs can then be used
to expose the iteration behavior without exposing the helper trait.

The following example implements this pattern for the `Tree<V>` type above as an
alternative to the simpler, but more duplicative, approach above.

```rust
mod tree {
    /// Binary tree with values at every node.
    /// Not necessarily balanced.
    pub struct Tree<V> {
        pub value: V,
        pub left: Option<Box<Tree<V>>>,
        pub right: Option<Box<Tree<V>>>,
    }

    impl<V> Tree<V> {
        // ... static methods for constructing trees ...
#
#        /// Constructs a new node with two
#        /// subtrees.
#        pub fn node(value: V, left: Tree<V>, right: Tree<V>) -> Tree<V> {
#            Tree {
#                value,
#                left: Some(Box::new(left)),
#                right: Some(Box::new(right)),
#            }
#        }
#
#        /// Constructs a new node with a left
#        /// subtree.
#        pub fn left(value: V, left: Tree<V>) -> Tree<V> {
#            Tree {
#                value,
#                left: Some(Box::new(left)),
#                right: None,
#            }
#        }
#
#        /// Constructs a new node with a right
#        /// subtree.
#        pub fn right(value: V, right: Tree<V>) -> Tree<V> {
#            Tree {
#                value,
#                left: None,
#                right: Some(Box::new(right)),
#            }
#        }
#
#        /// Constructs a new leaf node.
#        pub fn leaf(value: V) -> Self {
#            Tree {
#                value,
#                left: None,
#                right: None,
#            }
#        }
    }

    /// Internal trait for abstracting over access
    /// to the tree components.
    ///
    /// This reduces code duplication when
    /// implementing behavior that is essentially
    /// the same for Tree<V>, &Tree<V>,
    /// and &mut Tree<V>.
    trait Treeish: Sized {
        type Output;
        fn get(self) -> (Option<Self>, Self::Output, Option<Self>);
    }

    impl<V> Treeish for Tree<V> {
        type Output = V;
        fn get(self) -> (Option<Self>, Self::Output, Option<Self>) {
            let Tree { value, left, right } = self;
            (left.map(|x| *x), value, right.map(|x| *x))
        }
    }

    impl<'a, V> Treeish for &'a Tree<V> {
        type Output = &'a V;
        fn get(self) -> (Option<Self>, Self::Output, Option<Self>) {
            let Tree { value, left, right } = self;
            (left.as_deref(), value, right.as_deref())
        }
    }

    impl<'a, V> Treeish for &'a mut Tree<V> {
        type Output = &'a mut V;
        fn get(self) -> (Option<Self>, Self::Output, Option<Self>) {
            let Tree { value, left, right } = self;
            (left.as_deref_mut(), value, right.as_deref_mut())
        }
    }

    /// Internal struct for implementing Iterator
    /// in terms of Treeish.
    struct Preorder<T>(Vec<T>);

    impl<T> Iterator for Preorder<T>
    where
        T: Treeish,
    {
        type Item = T::Output;
        fn next(&mut self) -> Option<Self::Item> {
            let next = self.0.pop();
            match next {
                None => None,
                Some(t) => {
                    // the helper trait is used here
                    let (left, value, right) = t.get();
                    if let Some(right) = right {
                        self.0.push(right);
                    }
                    if let Some(left) = left {
                        self.0.push(left);
                    }
                    Some(value)
                }
            }
        }
    }

    // Wrappers for exposing the iterator. The wrappers are necessary
    // in order to keep Treeish private. Treeish::Output would
    // otherwise be exposed and thus require Treeish to be public.

    /// Preorder iterator
    pub struct IntoIterPreorder<V>(Preorder<Tree<V>>);

    /// Preorder iterator
    pub struct IterPreorder<'a, V>(Preorder<&'a Tree<V>>);

    /// Preorder iterator
    pub struct IterMutPreorder<'a, V>(Preorder<&'a mut Tree<V>>);

    // Delegate to the wrapped implementation.

    impl<V> Iterator for IntoIterPreorder<V> {
        type Item = V;
        fn next(&mut self) -> Option<Self::Item> {
            self.0.next()
        }
    }

    impl<'a, V> Iterator for IterPreorder<'a, V> {
        type Item = &'a V;
        fn next(&mut self) -> Option<Self::Item> {
            self.0.next()
        }
    }

    impl<'a, V> Iterator for IterMutPreorder<'a, V> {
        type Item = &'a mut V;
        fn next(&mut self) -> Option<Self::Item> {
            self.0.next()
        }
    }

    impl<V> Tree<V> {
        pub fn preorder(self) -> IntoIterPreorder<V> {
            IntoIterPreorder(Preorder(vec![self]))
        }

        pub fn preorder_ref(&self) -> IterPreorder<V> {
            IterPreorder(Preorder(vec![self]))
        }

        pub fn preorder_ref_mut(&mut self) -> IterMutPreorder<V> {
            IterMutPreorder(Preorder(vec![self]))
        }
    }
}

use tree::*;

fn main() {
    let mut t = Tree::node(
        0,
        Tree::left(1, Tree::leaf(2)),
        Tree::node(3, Tree::leaf(4), Tree::right(5, Tree::leaf(6))),
    );

    for x in t.preorder_ref_mut() {
        *x += 10;
    }

    for x in t.preorder_ref() {
        println!("{}", x);
    }
}
```

## Bidirectional and random access iterators

The Rust standard library does not include support for bidirectional or random
access iterators. For most of the use cases supported by those iterators,
working with numeric indices suffices.

The standard library does have support for [double-ended
iterators](https://doc.rust-lang.org/std/iter/trait.DoubleEndedIterator.html),
which allow consuming items from the back of the iterator. However, each item
can still only be consumed once: when the front and back meet in the middle,
iteration is over.

{{#quiz iterators.toml}}
