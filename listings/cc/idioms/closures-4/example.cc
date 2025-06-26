#include <iostream>
#include <string>

int main() {
  std::string x("");
  auto f = [&x]() {
    x.push_back('!');
    return x.size();
  };

  std::cout << f() << std::endl; // prints "1"
  std::cout << f() << std::endl; // prints "2"
}
