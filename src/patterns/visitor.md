# Visitor pattern and double dispatch

In C++ the visitor pattern is typically used to enable adding behaviors to a
type without modifying the class definitions. In Rust, the same goal is
conventionally accomplished by using Rust enums, which resemble C++ [tagged
unions](../idioms/data_modeling/tagged_unions.md). While the chapter on tagged
unions compares using Rust enums with C++ `std::variant`, this chapter [compares
using the visitor pattern in C++ with using Rust
enums](#use-a-rust-enum-instead).

Since the visitor pattern and double dispatch may be useful for other purposes
as well, a [Rust visitor pattern version of the example](#visitors) is also
given.

Extensions of the visitor pattern are sometimes used in C++ to make it possible
to extend both data and behavior without modifying the original definitions
(i.e., to solve [the expression
problem](https://cs.brown.edu/~sk/Publications/Papers/Published/kff-synth-fp-oo/)).
Other approaches, enabled by Rust's traits and generics, are [more likely to be
used in Rust](#varying-data-and-behavior).

## Use a Rust enum instead

For the first case, where the variants are fixed but behaviors are not, the
idiomatic approach in Rust is to implement the data structure as an enum instead
of as many structs with a common interface. This is similar to using
`std::variant` in C++.

<div class="comparison">

```cpp
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

// Declare types that visitor can visit
class Lit;
class Plus;
class Var;
class Let;

// Define abstract class for visitor
struct Visitor {
  virtual void visit(Lit &e) = 0;
  virtual void visit(Plus &e) = 0;
  virtual void visit(Var &e) = 0;
  virtual void visit(Let &e) = 0;
  virtual ~Visitor() = default;

protected:
  Visitor() = default;
};

// Define abstract class for expressions
struct Exp {
  virtual void accept(Visitor &v) = 0;
  virtual ~Exp() = default;
};

// Implement each expression variant
struct Lit : public Exp {
  int value;

  Lit(int value) : value(value) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }
};

struct Plus : public Exp {
  std::unique_ptr<Exp> lhs;
  std::unique_ptr<Exp> rhs;

  Plus(std::unique_ptr<Exp> lhs,
       std::unique_ptr<Exp> rhs)
      : lhs(std::move(lhs)), rhs(std::move(rhs)) {
  }

  void accept(Visitor &v) override {
    v.visit(*this);
  }
};

struct Var : public Exp {
  std::string name;

  Var(std::string name) : name(name) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }
};

struct Let : public Exp {
  std::string name;
  std::unique_ptr<Exp> exp;
  std::unique_ptr<Exp> body;

  Let(std::string name, std::unique_ptr<Exp> exp,
      std::unique_ptr<Exp> body)
      : name(std::move(name)),
        exp(std::move(exp)),
        body(std::move(body)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }
};

// Define Visitor for evaluating expressions

// Exception for representing expression
// evaluation errors
struct UnknownVar : std::exception {
  std::string name;

  UnknownVar(std::string name) : name(name) {}

  const char *what() const noexcept override {
    return "Unknown variable";
  }
};

// Define type for evaluation environment
using Env = std::unordered_map<std::string, int>;

// Define evaluator
struct EvalVisitor : public Visitor {
  // Return value. Results propagate up the stack.
  int value = 0;

  // Evaluation environment. Changes propagate
  // down the stack
  Env env;

  // Define behavior for each case of the
  // expression.
  void visit(Lit &e) override { value = e.value; }
  void visit(Plus &e) override {
    e.lhs->accept(*this);
    auto lhs = value;
    e.rhs->accept(*this);
    auto rhs = value;
    value = lhs + rhs;
  }
  void visit(Var &e) override {
    try {
      value = env.at(e.name);
    } catch (std::out_of_range &ex) {
      throw UnknownVar(e.name);
    }
  }
  void visit(Let &e) override {
    e.exp->accept(*this);
    auto orig_env = env;
    env[e.name] = value;
    e.body->accept(*this);
    env = orig_env;
  }
};

int main() {
  // Construct an expression
  auto x = Plus(std::make_unique<Let>(
                    std::string("x"),
                    std::make_unique<Lit>(3),
                    std::make_unique<Var>(
                        std::string("x"))),
                std::make_unique<Lit>(2));

  // Construct the evaluator
  EvalVisitor visitor;

  // Run the evaluator
  x.accept(visitor);

  // Print the output
  std::cout << visitor.value << std::endl;
}
```

```rust
use std::collections::HashMap;

// Define expressions.
//
// This covers the first 3 sections of the
// C++ version.
enum Exp {
    Var(String),
    Lit(i32),
    Plus {
        lhs: Box<Exp>,
        rhs: Box<Exp>,
    },
    Let {
        var: String,
        exp: Box<Exp>,
        body: Box<Exp>,
    },
}

// Exception for representing expression
// evaluation errors
#[derive(Debug)]
enum EvalError<'a> {
    UnknownVar(&'a str),
}

// Define type for evaluation environment
type Env<'a> = HashMap<&'a str, i32>;

// Define evaluator
fn eval<'a>(
    env: &Env<'a>,
    e: &'a Exp,
) -> Result<i32, EvalError<'a>> {
    match e {
        Exp::Var(x) => env
            .get(x.as_str())
            .cloned()
            .ok_or(EvalError::UnknownVar(x)),
        Exp::Lit(n) => Ok(*n),
        Exp::Plus { lhs, rhs } => {
            let lv = eval(env, lhs)?;
            let rv = eval(env, rhs)?;
            Ok(lv + rv)
        }
        Exp::Let { var, exp, body } => {
            let val = eval(env, exp)?;
            let mut env = env.clone();
            env.insert(var, val);
            eval(&env, body)
        }
    }
}

fn main() {
    use Exp::*;

    // Construct an expression
    let e = Let {
        var: "x".to_string(),
        exp: Box::new(Lit(3)),
        body: Box::new(Plus {
            lhs: Box::new(Var("x".to_string())),
            rhs: Box::new(Lit(2)),
        }),
    };

    // Run the evaluator
    let res = eval(&HashMap::new(), &e);

    // Print the output
    println!("{:?}", res);
}
```

</div>

## Visitors

If the visitor pattern is still needed for some reason, it can be implemented
similarly to how it is in C++. This can make direct ports of programs that use
the visitor pattern more feasible. However, the enum-based implementation should
still be preferred.

The following example shows how to implement the same program as in the previous
example, but using a visitor in Rust. The C++ program is identical to the
previous one.

The example also demonstrates using double dispatch with trait objects in Rust.
The expressions are represented as `dyn Exp` trait objects which accept a `dyn
Visitor` trait object, and then call on the visitor the method specific to the
type of expression.

<div class="comparison">

```cpp
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

// Declare types that visitor can visit
class Lit;
class Plus;
class Var;
class Let;

// Define abstract class for visitor
struct Visitor {
  virtual void visit(Lit &e) = 0;
  virtual void visit(Plus &e) = 0;
  virtual void visit(Var &e) = 0;
  virtual void visit(Let &e) = 0;
  virtual ~Visitor() = default;

protected:
  Visitor() = default;
};

// Define abstract class for expressions
struct Exp {
  virtual void accept(Visitor &v) = 0;
  virtual ~Exp() = default;
};

// Implement each expression variant
struct Lit : public Exp {
  int value;

  Lit(int value) : value(value) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }
};

struct Plus : public Exp {
  std::unique_ptr<Exp> lhs;
  std::unique_ptr<Exp> rhs;

  Plus(std::unique_ptr<Exp> lhs,
       std::unique_ptr<Exp> rhs)
      : lhs(std::move(lhs)), rhs(std::move(rhs)) {
  }

  void accept(Visitor &v) override {
    v.visit(*this);
  }
};

struct Var : public Exp {
  std::string name;

  Var(std::string name) : name(name) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }
};

struct Let : public Exp {
  std::string name;
  std::unique_ptr<Exp> exp;
  std::unique_ptr<Exp> body;

  Let(std::string name, std::unique_ptr<Exp> exp,
      std::unique_ptr<Exp> body)
      : name(std::move(name)),
        exp(std::move(exp)),
        body(std::move(body)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }
};

// Define Visitor for evaluating expressions

// Exception for representing expression
// evaluation errors
struct UnknownVar : std::exception {
  std::string name;

  UnknownVar(std::string name) : name(name) {}

  const char *what() const noexcept override {
    return "Unknown variable";
  }
};

// Define type for evaluation environment
using Env = std::unordered_map<std::string, int>;

// Define evaluator
struct EvalVisitor : public Visitor {
  // Return value. Results propagate up the stack.
  int value = 0;

  // Evaluation environment. Changes propagate
  // down the stack
  Env env;

  // Define behavior for each case of the
  // expression.
  void visit(Lit &e) override { value = e.value; }
  void visit(Plus &e) override {
    e.lhs->accept(*this);
    auto lhs = value;
    e.rhs->accept(*this);
    auto rhs = value;
    value = lhs + rhs;
  }
  void visit(Var &e) override {
    try {
      value = env.at(e.name);
    } catch (std::out_of_range &ex) {
      throw UnknownVar(e.name);
    }
  }
  void visit(Let &e) override {
    e.exp->accept(*this);
    auto orig_env = env;
    env[e.name] = value;
    e.body->accept(*this);
    env = orig_env;
  }
};

int main() {
  // Construct an expression
  auto x = Plus(std::make_unique<Let>(
                    std::string("x"),
                    std::make_unique<Lit>(3),
                    std::make_unique<Var>(
                        std::string("x"))),
                std::make_unique<Lit>(2));

  // Construct the evaluator
  EvalVisitor visitor;

  // Run the evaluator
  x.accept(visitor);

  // Print the output
  std::cout << visitor.value << std::endl;
}
```

```rust
// This is NOT an idiomatic translation. The
// previous example using Rust enums is.

use std::collections::HashMap;

// Define types that the visitor can visit
struct Lit(i32);
struct Plus {
    lhs: Box<dyn Exp>,
    rhs: Box<dyn Exp>,
}
struct Var(String);
struct Let {
    name: String,
    exp: Box<dyn Exp>,
    body: Box<dyn Exp>,
}

// Define trait for expressions
trait Exp {
    // Much like C++ can't have virtual template
    // methods, Rust can't have trait objects
    // where the traits have generic methods.
    //
    // Therefore the visitor either has to be
    // mutable to collect the results or the
    // accept method has to be specialized to a
    // specific return type.
    fn accept<'a>(&'a self, v: &mut dyn Visitor<'a>);
}

// Define trait for the visitor
trait Visitor<'a> {
    fn visit_lit(&mut self, e: &'a Lit);
    fn visit_plus(&mut self, e: &'a Plus);
    fn visit_var(&mut self, e: &'a Var);
    fn visit_let(&mut self, e: &'a Let);
}

// Implement accept behavior for each expression variant
impl Exp for Lit {
    fn accept<'a>(&'a self, v: &mut (dyn Visitor<'a>)) {
        v.visit_lit(self);
    }
}

impl Exp for Plus {
    fn accept<'a>(&'a self, v: &mut dyn Visitor<'a>) {
        v.visit_plus(self);
    }
}

impl Exp for Var {
    fn accept<'a>(&'a self, v: &mut dyn Visitor<'a>) {
        v.visit_var(self);
    }
}

impl Exp for Let {
    fn accept<'a>(&'a self, v: &mut dyn Visitor<'a>) {
        v.visit_let(self);
    }
}

// Define Visitor for evaluating expressions

// Error for representing expression evaluation
// errors.
//
// Has a lifetime parameter beacause it borrows
// the name from the expression.
#[derive(Debug)]
enum EvalError<'a> {
    UnknownVar(&'a str),
}

// Define type for evaluation environment
//
// Has a lifetime parameter because it borrows
// the names from the expression.
type Env<'a> = HashMap<&'a str, i32>;

// Define the evaluator
struct EvalVisitor<'a> {
    // Return value. Results propagate up the stack.
    env: Env<'a>,

    // Evaluation environment. Changes propagate
    // down the stack
    value: Result<i32, EvalError<'a>>,
}

// Define behavior for each case of the
// expression.
impl<'a> Visitor<'a> for EvalVisitor<'a> {
    fn visit_lit(&mut self, e: &'a Lit) {
        self.value = Ok(e.0);
    }

    fn visit_plus(&mut self, e: &'a Plus) {
        e.lhs.accept(self);
        let Ok(lv) = self.value else {
            return;
        };
        e.rhs.accept(self);
        let Ok(rv) = self.value else {
            return;
        };
        self.value = Ok(lv + rv);
    }

    fn visit_var(&mut self, e: &'a Var) {
        self.value = self
            .env
            .get(e.0.as_str())
            .ok_or(EvalError::UnknownVar(&e.0))
            .copied();
    }

    fn visit_let(&mut self, e: &'a Let) {
        e.exp.accept(self);
        let Ok(val) = self.value else {
            return;
        };
        let orig_env = self.env.clone();
        self.env.insert(e.name.as_ref(), val);
        e.body.accept(self);
        self.env = orig_env;
    }
}

fn main() {
    // Construct an expression
    let x = Plus {
        lhs: Box::new(Let {
            name: "x".to_string(),
            exp: Box::new(Lit(3)),
            body: Box::new(Var("x".to_string())),
        }),
        rhs: Box::new(Lit(2)),
    };

    // Construct the evaluator
    let mut visitor = EvalVisitor {
        value: Ok(0),
        env: HashMap::new(),
    };

    // Run the evaluator
    x.accept(&mut visitor);

    // Print the output
    println!("{:?}", visitor.value);
}
```

</div>

## Varying data and behavior

In C++, extensions to the visitor pattern are sometimes used to handle
situations where both data and behavior and vary. However, those solutions also
make use of dynamic casting. In Rust, that requires opting into
[RTTI](./../idioms/rtti.md) by making `Any` a supertrait of the trait for the
visitors, so they can be downcast. While this extension to the visitor pattern
is possible to implement, the ergonomics of the approach make other approaches
more common in Rust.

One of the alternative approaches, adopted from functional programming and
leveraging the design of traits and generics in Rust, is called ["data types à
la
carte"](https://www.cambridge.org/core/services/aop-cambridge-core/content/view/14416CB20C4637164EA9F77097909409/S0956796808006758a.pdf/data-types-a-la-carte.pdf).

The following example shows a variation on the earlier examples using this
pattern to make it so that two parts of the expression type can be defined
separately and given evaluators separately. This approach can lead to
performance problems (in large part due to the indirection through nested
structures) or increases in compilation time, so its necessity should be
carefully evaluated before it is used.

```rust
use std::collections::HashMap;

// A type for combining separately-defined
// expressions. Defining individual expressions
// completely separately and then using an
// application-specific sum type instead of nesting
// Sum can improve performance.
enum Sum<L, R> {
    Inl(L),
    Inr(R),
}

// Define arithmetic expressions
enum ArithExp<E> {
    Lit(i32),
    Plus { lhs: E, rhs: E },
}

// Define let bindings and variables
enum LetExp<E> {
    Var(String),
    Let { name: String, exp: E, body: E },
}

// Combine the expressions
type Sig<E> = Sum<ArithExp<E>, LetExp<E>>;

// Define the fixed-point for recursive
// expressions.
struct Exp(Sig<Box<Exp>>);

// Define an evaluator

// The evaluation environment
type Env<'a> = HashMap<&'a str, i32>;

// Evaluation errors
#[derive(Debug)]
enum EvalError<'a> {
    UndefinedVar(&'a str),
}

// A trait for expressions that can
// be evaluated.
trait Eval {
    fn eval<'a>(&'a self, env: &Env<'a>) -> Result<i32, EvalError<'a>>;
}

// Implement the evaluator trait for
// the administrative types

impl<L: Eval, R: Eval> Eval for Sum<L, R> {
    fn eval<'a>(&'a self, env: &Env<'a>) -> Result<i32, EvalError<'a>> {
        match self {
            Sum::Inl(left) => left.eval(env),
            Sum::Inr(right) => right.eval(env),
        }
    }
}

impl<E: Eval> Eval for Box<E> {
    fn eval<'a>(&'a self, env: &Env<'a>) -> Result<i32, EvalError<'a>> {
        self.as_ref().eval(env)
    }
}

// Implement the trait for the desired variants.
impl<E: Eval> Eval for ArithExp<E> {
    fn eval<'a>(&'a self, env: &Env<'a>) -> Result<i32, EvalError<'a>> {
        match self {
            ArithExp::Lit(n) => Ok(*n),
            ArithExp::Plus { lhs, rhs } => Ok(lhs.eval(env)? + rhs.eval(env)?),
        }
    }
}

impl<E: Eval> Eval for LetExp<E> {
    fn eval<'a>(&'a self, env: &Env<'a>) -> Result<i32, EvalError<'a>> {
        match self {
            LetExp::Var(x) => env
                .get(x.as_str())
                .copied()
                .ok_or(EvalError::UndefinedVar(x)),
            LetExp::Let { name, exp, body } => {
                let arg = exp.eval(env)?;
                let mut env = env.clone();
                env.insert(name, arg);
                body.eval(&env)
            }
        }
    }
}

// Since the trait is implemented for everything
// inside of Exp, it can be implemented for Exp.
impl Eval for Exp {
    fn eval<'a>(&'a self, env: &Env<'a>) -> Result<i32, EvalError<'a>> {
        self.0.eval(env)
    }
}

// helpers for constructing expressions

fn lit(n: i32) -> Exp {
    Exp(Sum::Inl(ArithExp::Lit(n)))
}

fn plus(lhs: Exp, rhs: Exp) -> Exp {
    Exp(Sum::Inl(ArithExp::Plus {
        lhs: Box::new(lhs),
        rhs: Box::new(rhs),
    }))
}

fn var(x: &str) -> Exp {
    Exp(Sum::Inr(LetExp::Var(x.to_string())))
}

fn elet(name: &str, val: Exp, body: Exp) -> Exp {
    Exp(Sum::Inr(LetExp::Let {
        name: name.to_string(),
        exp: Box::new(val),
        body: Box::new(body),
    }))
}

fn main() {
    let e = elet("x", lit(3), plus(var("x"), lit(2)));

    println!("{:?}", e.eval(&HashMap::new()));
}
```

One thing worth noting about the above implementation is that no dynamic
dispatch was required.

{{#quiz visitor.toml}}
