#include <iostream>
#include <string>

int main() {
  std::string x("hi");
  auto f = [x = std::move(x)]() mutable {
    return std::move(x);
  };

  std::cout << f() << std::endl;
  // compiles, but the captured value has been
  // moved
  std::cout << f() << std::endl; // prints ""
}
