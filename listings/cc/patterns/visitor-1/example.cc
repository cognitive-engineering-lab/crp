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
