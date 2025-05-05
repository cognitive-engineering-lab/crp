# Rule of three/five/zero

## Rule of three

In C++ the rule of three is a rule of thumb that if a class has a user-defined
destructor, copy constructor or copy assignment operator, it probably should
have all three.

The corresponding rule for Rust is that if a type has a user-defined `Clone` or
`Drop` implementation, it probably needs both. This is for the same reason as
the rule of three in C++: if a type has a user-defined implementation for
`Clone` or `Drop`, it is probably because the type manages a resource, and both
`Clone` and `Drop` will need to take special actions for the resource.

## Rule of five

The rule of five in C++ states that if move semantics are needed for a type with
a user-defined copy constructor or copy assignment operator, then a user-defined
move constructor and move assignment should also be provided, because no
implicit move constructor or move assignment operator will be generated.

In Rust, this rule does not apply because of the [difference in move semantics
between C++ and Rust.](copy_and_move_constructors.md#move-constructors)

## Rule of zero

The rule of zero states that classes with user-defined copy/move constructors,
assignment operators, and destructors should deal only with ownership, and other
classes should not have those constructors or destructors. In practice, most
classes should make use of types from the STL (`shared_ptr`, `vector`, etc.) for
dealing with ownership concerns so that the implicitly defined copy and move
constructors are sufficient.

In Rust, the same is true. See the list of Rust type equivalents for equivalents
of C++ [smart pointer types](/idioms/type_equivalents.md#pointers) and
equivalents of C++ [container types](/idioms/type_equivalents.md#containers).

One difference between C++ and Rust in applying the rule of zero is that in C++
`std::unique_ptr` can take a custom deleter, making it possible to use
`std::unique_ptr` for wrapping raw pointers that require custom destruction
logic. In Rust, the `Box` type is not parameterized in the same way. To
accomplish the same goal, one instead must define a new type with a user-defined
`Drop` implementation, as is done in [the example in the chapter on copy and
move
constructors](/idioms/constructors/copy_and_move_constructors.md#user-defined-constructors).
