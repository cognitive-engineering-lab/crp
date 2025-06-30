# Data modeling

In C++ the mechanisms available for data modeling are classes, enums, and
unions.

Rust, on the other hand, uses records
([structs](https://doc.rust-lang.org/book/ch05-00-structs.html)) and algebraic
data types ([enums](https://doc.rust-lang.org/book/ch06-00-enums.html)).

Although Rust supports one major piece of object oriented design, polymorphism
using interfaces, Rust also has language features for modeling things using
algebraic data types (which in simple cases are like a much more ergonomic
`std::variant`).

This section gives examples of common constructions used when programming in C++
and how to achieve the same effects using Rust's features.

## Fixed operations, varying data

In situations where one needs to model a fixed set of operations that clients
will use, but the data that implements those operations are not fixed ahead of
time, the approach in C++ and the approach in Rust are the same. In both cases
interfaces that define the required operations are defined. Concrete types,
possibly defined by the client, implement those interfaces.

This way of modeling data can make use of either
[dynamic](./data_modeling/abstract_classes.md) or [static
dispatch](./data_modeling/concepts.md), each of which is covered in its
own section.

## Fixed data, varying operations

In situations where there is a fixed set of data but the operations that the
data must support vary, there are a few approaches in C++. Which approaches are
available to use depend on the version of the standard in use.

In older versions of the standard, one might use manually defined tagged unions.
In newer versions, `std::variant` is available to improve the safety and
ergonomics of tagged unions. [Both of these approaches map to the same approach
in Rust](./data_modeling/tagged_unions.md).

Additionally, despite it not being strictly necessary to model a fixed set of
variants, the visitor pattern is sometimes used for this situation, especially
when using versions of the C++ standard before the introduction of
`std::variant`. In most of these cases the idiomatic Rust solution is the same
as what one would do when converting a C++ solution that uses [tagged
unions](./data_modeling/tagged_unions.md). The chapter on the [visitor
pattern](../patterns/visitor.md) describes when to use a Rust version of the
visitor pattern or when to use Rust's enums (which are closer to `std::variant`
than to C++ enums) to model the data.

## Varying data and operations

When both data and operations may be extended by a client, the required
solutions are more complex. In C++, the approach usually involves some kind of
extension to the [visitor pattern](../patterns/visitor.md) along with dynamic
casting. Because Rust does not support the kind of RTTI necessary for a dynamic
cast operator, different approaches need to be used. Some of those approaches
are discussed in the [chapter on the visitor
pattern](../patterns/visitor.md#expression-problem).
