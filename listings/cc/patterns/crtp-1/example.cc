#include <iostream>
#include <span>
#include <string>
#include <vector>

// D is the type of the derived class
template <typename D>
struct Combinable {
  D combineWith(D &d);

  // concat is implemented in the base class, but
  // operates on values of the derived class.
  D concat(std::span<D> vec) {
    D acc(*static_cast<D *>(this));

    for (D &v : vec) {
      acc = acc.combineWith(v);
    }

    return acc;
  }
};

struct Sum : Combinable<Sum> {
  int sum;

  Sum(int sum) : sum(sum) {}

  Sum combineWith(Sum s) {
    return Sum(sum + s.sum);
  }

  // Sum includes an additional method that can be
  // chained.
  Sum mult(int n) {
    return Sum(sum * n);
  }
};

int main() {
  Sum s(0);
  std::vector<Sum> v{1, 2, 3, 4};
  Sum x = s.concat(v)
              // Even though concat is part of the
              // base class, it returns a value of
              // the implementing class, making it
              // possible to chain methods
              // specific to that class.
              .mult(2)
              .combineWith(5);
  std::cout << x.sum << std::endl;
}
