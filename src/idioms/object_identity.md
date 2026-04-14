# Object identity

In C++ the pointer to an object is sometimes used to represent its identity in
terms of the logic of a program.

In some cases, this is a standard optimization, such as when implementing the
copy assignment operator.

In other cases the pointer value is used as a logical identity to distinguish
between specific instances of an object that otherwise have the same properties.
For example, representing a labeled graph where there may be distinct nodes that
have the same label.

In Rust, some of these cases are not applicable, and other cases are typically
handled instead by implementing a synthetic notion of identity for the
values.

## Overloading copy assignment and equality comparison operators

For example, when implementing the copy-assignment operator, one might
short-circuit when the copied object and the assignee are the same.
Note that in this use the pointer values are not stored.

This kind of optimization is unnecessary when implementing [Rust's equivalent to
the copy assignment
operator](./constructors/copy_and_move_constructors.md#assignment-operators)
`Clone::clone_from`. The type of `Clone::clone_from` prevents the same object
from being passed as both arguments, because one of the arguments is a mutable
reference, which is exclusive, and so prevents the other reference argument from
referring to the same object.

<div class="comparison">

```cpp
struct Person
{
    std::string name;
    // many other expensive-to-copy fields

    Person& operator=(const Person& other) {
        // compare object identity first
        if (this != &other) {
            this->name = other.name;
            // copy the other expensive-to-copy fields
        }

        return *this;
    }
};
```

```rust
struct Person {
    name: String,
}

impl Clone for Person {
    fn clone(&self) -> Self {
        Self { name: self.name.clone() }
    }

    fn clone_from(&mut self, source: &Self) {
        // self and source cannot be the same here,
        // because that would mean there are a
        // mutable and an immutable reference to
        // the same memory location. Therefore, a
        // check for assignment to self is not
        // needed, even for the purpose of
        // optimization.

        self.name.clone_from(&source.name);
    }
}
```

</div>

In cases in C++ where most comparisons are between an object and itself (e.g.,
the object's primary use is to be stored in a hash set), and comparison of
unequal objects is expensive, comparing object identity might be used as
optimization for the equality comparison operator overload.

For supporting similar operations in Rust,
[`std::ptr::eq`](https://doc.rust-lang.org/std/ptr/fn.eq.html) can be used.

<div class="comparison">

```cpp
struct Person
{
    std::string name;
    // many other expensive-to-compare fields
};


bool operator==(const Person& lhs, const Person& rhs) {
    // compare object identity first
    if (&lhs == &rhs) {
        return true;
    }

    // compare the other expensive-to-compare fields

    return true;
}
```

```rust
struct Person {
    name: String,
    // many other expensive-to-compare fields
}

impl PartialEq for Person {
    fn eq(&self, other: &Self) -> bool {
        if std::ptr::eq(self, other) {
            return true;
        }
        // compare other expensive-to-compare fields

        true
    }
}

impl Eq for Person {}
```

</div>

## Distinguishing between values in a relational structure

The other use is when relationships between values are represented using a data
structure external to the values, such as when representing a labeled graph in
which multiple nodes might share the same label, but have edges between
different sets of other nodes. This differs from the earlier case because the
pointer value is preserved.

One real-world example of this is in the LLVM codebase, where occurrences of
declarations, statements, and expressions in the AST are distinguished by object
identity. For example, variable expressions (`class DeclRefExpr`) contain the
[pointer to the occurrence of the declaration to which the variable
refers](https://github.com/llvm/llvm-project/blob/ddc48fefe389789f64713b5924a03fb2b7961ef3/clang/include/clang/AST/Expr.h#L1265C1-L1275C16).

Similarly, when comparing whether two variable declarations represent
declarations of the same variable, [a pointer to some canonical `VarDecl` is
used](https://github.com/llvm/llvm-project/blob/aa33c095617400a23a2b814c4defeb12e7761639/clang/lib/AST/Stmt.cpp#L1476-L1485):

```cpp
VarDecl *VarDecl::getCanonicalDecl();

bool CapturedStmt::capturesVariable(const VarDecl *Var) const {
  for (const auto &I : captures()) {
    if (!I.capturesVariable() && !I.capturesVariableByCopy())
      continue;
    if (I.getCapturedVar()->getCanonicalDecl() == Var->getCanonicalDecl())
      return true;
  }

  return false;
}
```

This kind of use is often discouraged in C++ because of the risk of
use-after-free bugs, but might be used in performance sensitive applications
where either storing the memory to represent the mapping or the additional
indirection to resolve an entity's value from its identity is cost prohibitive.

In Rust it is generally preferred to represent the identity of the objects with
synthetic identifiers. This is in part as a technique for modeling
self-referential data structures.

As an example, one popular Rust graph library
[petgraph](https://docs.rs/petgraph/latest/petgraph/) uses `u32` as its default
node identity type. This incurs the cost of an extra call to dereference the
synthetic identifier to the label of the represented node as well as the extra
memory required to store the mapping from nodes to labels.

A simplified graph representation using the same synthetic identifier technique
would look like the following, which represents the node identities by their
index in the vectors that represent the labels and the edges.

```rust
enum Color {
    Red,
    Blue
}

struct Graph {
    /// Maps from node id to node labels, which here are colors.
    nodes_labels: Vec<Color>,

    /// Maps from node id to adjacent nodes ids.
    edges: Vec<Vec<usize>>,
}
```

If performance requirements make the use of synthetic identifiers unacceptable,
then it may be necessary to use prevent the value from being moved. The [`Pin`
and `PhantomPinned` structs](https://doc.rust-lang.org/std/pin/index.html) can
be used to achieve an effect similar to deleting the move constructor in C++.

{{#quiz object_identity.toml}}
