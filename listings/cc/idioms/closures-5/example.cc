#include <iostream>
#include <string>

int main() {
  std::string x("");
  auto f = [&x]() { return x.size(); };

  std::cout << f() << std::endl; // prints "0"
  std::cout << f() << std::endl; // prints "0"
}
