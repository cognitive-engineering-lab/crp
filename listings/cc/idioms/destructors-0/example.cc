#include <iostream>
#include <utility>

struct A {
  int id;

  A(int id) : id(id) {}

  // copy constructor
  A(A &other) : id(other.id) {}

  // move constructor
  A(A &&other) : id(other.id) {
    other.id = 0;
  }

  // destructor
  ~A() {
    std::cout << id << std::endl;
  }
};

int accept(A x) {
  return x.id;
} // the destructor of x is called after the
  // return expression is evaluated

// Prints:
// 2
// 3
// 0
// 1
int main() {
  A x(1);
  A y(2);

  accept(std::move(y));

  A z(3);

  return 0;
}
