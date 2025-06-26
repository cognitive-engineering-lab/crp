#include <iostream>
#include <memory>
#include <string>

// The common interface
struct Debug {
  virtual std::ostream &
  emit(std::ostream &out) const = 0;
};

std::ostream &operator<<(std::ostream &out,
                         const Debug &d) {
  d.emit(out);
  return out;
}

// Two things that implement the interface
struct A : public Debug {
  std::ostream &
  emit(std::ostream &out) const override {
    out << "A";
    return out;
  }
};

struct B : public Debug {
  std::ostream &
  emit(std::ostream &out) const override {
    out << "B";
    return out;
  }
};

int main() {
  // Without the return-type annotation,
  // std::unique_ptr<A> would be inferred.
  auto f = [](std::unique_ptr<A> a,
              std::unique_ptr<B> b)
      -> std::unique_ptr<Debug> { return a; };
  std::cout << *f(std::make_unique<A>(),
                  std::make_unique<B>())
            << std::endl;
}
