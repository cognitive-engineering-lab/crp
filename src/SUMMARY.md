# Summary

[C++ to Rust Phrasebook](./title-page.md)

# Idioms

- [Constructors](./idioms/constructors.md)
  - [Default constructors](./idioms/constructors/default_constructors.md)
  - [Copy and move constructors](./idioms/constructors/copy_and_move_constructors.md)
  - [Rule of three/five/zero](./idioms/constructors/rule_of_three_five_zero.md)
  <!-- - [Separate construction and initialization](./idioms/constructors/partial_initialzation.md) -->
- [Destructors and resource cleanup](./idioms/destructors.md)
- [Data modeling](./idioms/data_modeling.md)
  - [Abstract classes, interfaces, and dynamic dispatch](./idioms/data_modeling/abstract_classes.md)
  - [Concepts, interfaces, and static dispatch](./idioms/data_modeling/concepts.md)
  - [Enums](./idioms/data_modeling/enums.md)
  - [Tagged unions and `std::variant`](./idioms/data_modeling/tagged_unions.md)
  - [Inheritance and implementation reuse](./idioms/data_modeling/inheritance_and_reuse.md)
  - [Template classes, functions, and methods](./idioms/data_modeling/templates.md)
  - [Template specialization](./idioms/data_modeling/template_specialization.md)
- [Null (`nullptr`)](./idioms/null.md)
  - [Sentinel values](./idioms/null/sentinel_values.md)
  - [Moved members](./idioms/null/moved_members.md)
  - [Zero-length arrays](./idioms/null/zero_length_arrays.md)
- [Encapsulation](./idioms/encapsulation.md)
  - [Header files](./idioms/encapsulation/headers.md)
  - [Anonymous namespaces and `static`](./idioms/encapsulation/anonymous_namespaces.md)
  - [Private members and friends](./idioms/encapsulation/private_and_friends.md)
  - [Private constructors](./idioms/encapsulation/private_constructors.md)
  - [Setter and getter methods](./idioms/encapsulation/setters_and_getters.md)
- [Exceptions and error handling](./idioms/exceptions.md)
  - [Expected errors](./idioms/exceptions/expected_errors.md)
  - [Errors indicating bugs](./idioms/exceptions/bugs.md)
- [Type equivalents](./idioms/type_equivalents.md)
- [Type promotions and conversions](./idioms/promotions_and_conversions.md)
- [User-defined conversions](./idioms/user-defined_conversions.md)
- [Overloading](./idioms/overloading.md)
- [RTTI and `dynamic_cast`](./idioms/rtti.md)
- [Iterators]()
- [Lambdas, closures,and function objects](./idioms/closures.md)
- [Object identity](./idioms/object_identity.md)
- [Out parameters](./idioms/out_params.md)
  - [Multiple return values](./idioms/out_params/multiple_return.md)
  - [Optional return values](./idioms/out_params/optional_return.md)
  - [Pre-allocated buffers](./idioms/out_params/pre-allocated_buffers.md)
- [Varargs]()
- [Attributes]()
- [Calling C (FFI)]()
- [NRVO, RVO, and placement new]()
- [Concurrency (threads and async)]()

# Patterns

- [Adapter pattern]()
- [Visitor pattern and double dispatch]()
- [Curiously recurring template pattern (CRTP)](./patterns/crtp.md)
- [Pointer-to-implementation (PImpl)]()
- [X macros]()

# Ecosystem

- [Libraries](./etc/libraries.md)
- [Unit tests]()
- [Documentation (Doxygen)]()
- [Build systems (CMake)]()

---

- [Attribution notices](./notices.md)
