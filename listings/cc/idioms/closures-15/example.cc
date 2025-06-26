#include <cassert>
#include <functional>
#include <iostream>

struct Interface {
  virtual void showVirtual() = 0;
};

struct A : public Interface {
  void show() {
    std::cout << "A" << std::endl;
  }

  void showVirtual() override {
    std::cout << "A" << std::endl;
  }
};

struct B : public Interface {
  void showVirtual() override {
    std::cout << "B" << std::endl;
  }

  void show() {
    std::cout << "B" << std::endl;
  }
};

int main() {
  auto showV = &Interface::showVirtual;
  auto showA = &A::show;
  auto showB = &B::show;

  A a;
  B b;

  (a.*showV)(); // prints A
  (b.*showV)(); // prints B

  (a.*showA)(); // prints A
  (b.*showB)(); // prints B
}
