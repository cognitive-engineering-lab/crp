# Template classes, functions, and methods

The most common uses of templates in C++ are to define classes, methods, traits,
or functions that work for any type (or at least for any type that provides
certain methods). This use case is common in the STL for container classes (such
as `<vector>`) and for the algorithms library (`<algorithm>`).

The following example defines a template for a directed graph represented as an
adjacency list, where the graph is generic in the type of the labels on the
nodes. Though the example shows a template class, the same comparisons with Rust
apply to template methods and template functions.

The same kind of reusable code can be created in Rust using generic types.

<div class="comparison">

```cpp
#include <stdexcept>
#include <vector>

template <typename Label>
class DirectedGraph {
  std::vector<std::vector<size_t>> adjacencies;
  std::vector<Label> nodeLabels;

public:
  size_t addNode(Label label) {
    adjacencies.push_back(std::vector<size_t>());
    nodeLabels.push_back(label);
    return numNodes() - 1;
  }

  void addEdge(size_t from, size_t to) {
    size_t numNodes = this->numNodes();
    if (from >= numNodes || to >= numNodes) {
      throw std::invalid_argument(
          "Node index out of range");
    }
    adjacencies[from].push_back(to);
  }

  size_t numNodes() const {
    return adjacencies.size();
  }
};
```

```rust
pub struct DirectedGraph<Label> {
    adjacencies: Vec<Vec<usize>>,
    node_labels: Vec<Label>,
}

impl<Label> DirectedGraph<Label> {
    pub fn new() -> Self {
        DirectedGraph {
            adjacencies: Vec::new(),
            node_labels: Vec::new(),
        }
    }

    pub fn add_node(
        &mut self,
        label: Label,
    ) -> usize {
        self.adjacencies.push(Vec::new());
        self.node_labels.push(label);
        self.num_nodes() - 1
    }

    pub fn add_edge(
        &mut self,
        from: usize,
        to: usize,
    ) -> Result<(), &str> {
        let num_nodes = self.num_nodes();
        if from >= num_nodes || to >= num_nodes {
            Err("Node index out of range.")
        } else {
            self.adjacencies[from].push(to);
            Ok(())
        }
    }

    pub fn num_nodes(&self) -> usize {
        self.node_labels.len()
    }
}
```

</div>

In the use case demonstrated in the above example, there are few practical
differences between using C++ template to define a class and using and Rust's
generics to define a struct. Whenever one would use a template that takes a
`typename` or `class` parameter in C++, one can instead take a type parameter in
Rust.

## Operations on the parameterized type

The differences become more apparent when one attempts to perform operations on
the values. The following code listing adds a method to get the smallest node in
the graph to both the Rust and the C++ examples.

<div class="comparison">

```cpp
#include <optional>
$#include <stdexcept>
$#include <vector>

template <typename Label>
class DirectedGraph {
$  std::vector<std::vector<size_t>> adjacencies;
$  std::vector<Label> nodeLabels;
$
public:
$  size_t addNode(Label label) {
$    adjacencies.push_back(std::vector<size_t>());
$    nodeLabels.push_back(label);
$    return numNodes() - 1;
$  }
$
$  void addEdge(size_t from, size_t to) {
$    size_t numNodes = this->numNodes();
$    if (from >= numNodes || to >= numNodes) {
$      throw std::invalid_argument(
$          "Node index out of range");
$    }
$    adjacencies[from].push_back(to);
$  }
$
$  size_t numNodes() const {
$    return adjacencies.size();
$  }
$
  std::optional<size_t> smallestNode() {
    if (nodeLabels.empty()) {
      return std::nullopt;
    }
    Label &least = nodeLabels[0];
    size_t index = 0;

    for (int i = 1; i < nodeLabels.size(); i++) {
      if (least > nodeLabels[i]) {
        least = nodeLabels[i];
        index = i;
      }
    }
    return std::optional(index);
  }
};
```

```rust
# pub struct DirectedGraph<Label> {
#     adjacencies: Vec<Vec<usize>>,
#     node_labels: Vec<Label>,
# }
#
impl<Label> DirectedGraph<Label> {
#     pub fn new() -> Self {
#         DirectedGraph {
#             adjacencies: Vec::new(),
#             node_labels: Vec::new(),
#         }
#     }
#
#     pub fn add_node(
#         &mut self,
#         label: Label,
#     ) -> usize {
#         self.adjacencies.push(Vec::new());
#         self.node_labels.push(label);
#         self.num_nodes() - 1
#     }
#
#     pub fn num_nodes(&self) -> usize {
#         self.node_labels.len()
#     }
#
#     pub fn add_edge(
#         &mut self,
#         from: usize,
#         to: usize,
#     ) -> Result<(), &str> {
#         if from > self.num_nodes()
#             || to > self.num_nodes()
#         {
#             Err("Node not in graph.")
#         } else {
#             self.adjacencies[from].push(to);
#             Ok(())
#         }
#     }
    pub fn smallest_node(&self) -> Option<usize>
    where
        Label: Ord,
    {
        // Matches the C++, but is not the idomatic
        // implementation!
        if self.node_labels.is_empty() {
            None
        } else {
            let mut least = &self.node_labels[0];
            let mut index = 0;
            for i in 1..self.node_labels.len() {
                if *least > self.node_labels[i] {
                    least = &self.node_labels[i];
                    index = i;
                }
            }
            Some(index)
        }
    }
}
```

</div>

The major difference between these implementations is that in the C++ version
`operator>` is used on the values without knowing whether the operator is
defined for the type. In the Rust version, there is a constraint requiring that
the `Label` type implement the `Ord` trait. (See the chapter on [concepts,
interfaces, and static dispatch](./concepts.md) for more
details on Rust traits and how they relate to C++ concepts.)

Unlike C++ templates, generic definitions in Rust are type checked at the point
of definition rather than at the point of use. This means that for operations to
be used on values with the type of a type parameter, the parameter has to be
constrained to types that implement some trait. As can be seen in the above
example, much like with C++ concepts and `requires`, the constraint can be
required for individual methods rather than for the whole generic class.

It is best practice in Rust to put the trait bounds on the specific things that
require the bounds, in order to make the overall use of the types more flexible.

As an aside, a more idiomatic implementation of `smallest_node` makes use of
Rust's iterators. This style of implementation may take some getting used to for
programmers more accustomed to implementations in the style used in the earlier
example.

```rust
# pub struct DirectedGraph<Label> {
#     adjacencies: Vec<Vec<usize>>,
#     node_labels: Vec<Label>,
# }
#
impl<Label> DirectedGraph<Label> {
#     pub fn new() -> Self {
#         DirectedGraph {
#             adjacencies: Vec::new(),
#             node_labels: Vec::new(),
#         }
#     }
#
#     pub fn add_node(
#         &mut self,
#         label: Label,
#     ) -> usize {
#         self.adjacencies.push(Vec::new());
#         self.node_labels.push(label);
#         self.num_nodes() - 1
#     }
#
#     pub fn num_nodes(&self) -> usize {
#         self.node_labels.len()
#     }
#
#     pub fn add_edge(
#         &mut self,
#         from: usize,
#         to: usize,
#     ) -> Result<(), &str> {
#         if from > self.num_nodes()
#             || to > self.num_nodes()
#         {
#             Err("Node not in graph.")
#         } else {
#             self.adjacencies[from].push(to);
#             Ok(())
#         }
#     }
    pub fn smallest_node(&self) -> Option<usize>
    where
        Label: Ord,
    {
        self.node_labels
            .iter()
            .enumerate()
            .map(|(i, l)| (l, i))
            .min()
            .map(|(_, i)| i)
    }
}
```

An even more idiomatic implementation would make use of the [itertools
crate](https://docs.rs/itertools/latest/itertools/trait.Itertools.html#method.position_min).

```rust,ignore
use itertools::*;

# pub struct DirectedGraph<Label> {
#     adjacencies: Vec<Vec<usize>>,
#     node_labels: Vec<Label>,
# }
#
impl<Label> DirectedGraph<Label> {
#     pub fn new() -> Self {
#         DirectedGraph {
#             adjacencies: Vec::new(),
#             node_labels: Vec::new(),
#         }
#     }
#
#     pub fn add_node(
#         &mut self,
#         label: Label,
#     ) -> usize {
#         self.adjacencies.push(Vec::new());
#         self.node_labels.push(label);
#         self.num_nodes() - 1
#     }
#
#     pub fn num_nodes(&self) -> usize {
#         self.node_labels.len()
#     }
#
#     pub fn add_edge(
#         &mut self,
#         from: usize,
#         to: usize,
#     ) -> Result<(), &str> {
#         if from > self.num_nodes()
#             || to > self.num_nodes()
#         {
#             Err("Node not in graph.")
#         } else {
#             self.adjacencies[from].push(to);
#             Ok(())
#         }
#     }
#
    pub fn smallest_node(&self) -> Option<usize>
    where
        Label: Ord,
    {
        self.node_labels.iter().position_min()
    }
}
```

## `constexpr` template parameters

Rust also supports the equivalent of constexpr template parameters. For example,
one can define a generic function that returns an array of consecutive integers
starting from a specific value and whose size is determined at compile time.

<div class="comparison">

```cpp
#include <array>
#include <cstddef>

template <size_t N>
std::array<int, N>
makeSequentialArray(int start) {
  std::array<int, N> arr;
  for (size_t i = 0; i < N; i++) {
    arr[i] = start + i;
  }
}
```

```rust
fn make_sequential_array<const N: usize>(
    start: i32,
) -> [i32; N] {
    std::array::from_fn(|i| start + i as i32)
}
```

</div>

The corresponding idiomatic Rust function uses the helper `std::array::from_fn`
to construct the array. `from_fn` itself takes as type parameters the element
type and the constant. Those arguments are elided because Rust can infer them,
because both are part of the type of the produced array.

## Rust's `Self` type

Within a Rust struct defintion, `impl` block, or `impl` trait block, there is a
`Self` type that is in scope. The `Self` type is the type of the class being
defined with all of the generic type parameters filled in. It can be useful to
refer to this type especially in cases where there are many parameters that
would otherwise have to be listed out.

The `Self` type is necessary when defining generic traits to refer to the
concrete implementing type. Because Rust does not have inheritance between
concrete types and does not have method overriding, this is sufficient to avoid
the need to pass the implementing type as a type parameter.

For examples of this, see the chapter on the [curiously reoccurring template
pattern](../../patterns/crtp.md#method-chaining).

## A note on type checking and type errors

The checking of generic types at the point of definition rather than at the
point of template expansion impacts when errors are detected and how they are
reported. Some of this difference cannot be achieved by consistently using C++
concepts to declare the operations required.

For example, one might accidentally make the `nodeLabels` member a vector of
`size_t` instead of a vector of the label parameter. If all of the test cases
for the graph used label types that were convertible to integers, the error
would not be detected.

A similar Rust program fails to compile, even without a function that
instantiates the generic structure with a concrete type.

<div class="comparison">

```cpp
#include <stdexcept>
#include <vector>

template <typename Label>
class DirectedGraph {
  // The mistake is here: size_t should be Label
  std::vector<std::vector<size_t>> adjacencies;
  std::vector<size_t> nodeLabels;

public:
  Label getNode(size_t nodeId) {
    return nodeLabels[nodeId];
  }

  size_t addNode(Label label) {
    adjacencies.push_back(std::vector<size_t>());
    nodeLabels.push_back(label);
    return numNodes() - 1;
  }

  size_t numNodes() const {
    return adjacencies.size();
  }
};

#define BOOST_TEST_MODULE DirectedGraphTests
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_add_node_int) {
  DirectedGraph<int> g;
  auto n1 = g.addNode(1);
  BOOST_CHECK_EQUAL(1, g.getNode(n1));
}

BOOST_AUTO_TEST_CASE(test_add_node_float) {
  DirectedGraph<float> g;
  float label = 1.0f;
  auto n1 = g.addNode(label);
  BOOST_CHECK_CLOSE(label, g.getNode(n1), 0.0001);
}
```

```rust,ignore
pub struct DirectedGraph<Label> {
    // The mistake is here: size_t should be Label
    adjacencies: Vec<Vec<usize>>,
    node_labels: Vec<usize>,
}

impl<Label> DirectedGraph<Label> {
    pub fn new() -> Self {
        DirectedGraph {
            adjacencies: Vec::new(),
            node_labels: Vec::new(),
        }
    }

    pub fn get_node(
        &self,
        node_id: usize,
    ) -> Option<&Label> {
        self.node_labels.get(node_id)
    }

    pub fn add_node(
        &mut self,
        label: Label,
    ) -> usize {
        self.adjacencies.push(Vec::new());
        self.node_labels.push(label);
        self.num_nodes() - 1
    }

    pub fn num_nodes(&self) -> usize {
        self.node_labels.len()
    }
}
```

</div>

Despite the error, the C++ example compiles and passes the tests.

```text
Running 2 test cases...

*** No errors detected
```

Even without test cases, the Rust example fails to compile and produces a
message useful for identifying the error.

```text
error[E0308]: mismatched types
    --> example.rs:26:31
     |
6    | impl<Label> DirectedGraph<Label> {
     |      ----- found this type parameter
...
26   |         self.node_labels.push(label);
     |                          ---- ^^^^^ expected `usize`, found type parameter `Label`
     |                          |
     |                          arguments to this method are incorrect
     |
     = note:        expected type `usize`
             found type parameter `Label`
```

## Lifetimes parameters

Rust's generics are also used for classes, methods, traits, and functions that
are generic in the lifetimes of the references they manipulate. Unlike other
type parameters, the using a function with different lifetimes does not cause
additional copies of the function to be generated in the compiled code, because
lifetimes do not impact the runtime representation.

The chapter on concepts includes [examples of how lifetimes interact with Rust's
generics](./concepts.md#generics-and-lifetimes).

## Conditional compilation

One significant difference between C++ templates and Rust generics is that C++
templates are actually a more general purpose macro language, supporting things
like conditional compilation (e.g., when used in conjunction with `if
constexpr`, `requires`, or `std::enable_if`). Rust supports these use cases with
its macro system, which differs significantly from C++. The most common use of
the macro system, conditional compilation, is provided by [the `cfg` attribute
and `cfg!` macro](https://doc.rust-lang.org/rust-by-example/attribute/cfg.html).

The separation of conditional compilation from generics in Rust involves similar
design considerations as the omission of [template
specialization](./template_specialization.md) from Rust.

{{#quiz templates.toml}}
