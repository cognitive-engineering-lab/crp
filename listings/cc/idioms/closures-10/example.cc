#include <iostream>
#include <string>

int main() {
  std::string x("hello world");
  std::string y("goodnight moon");

  auto f = [&]() {
    x.push_back('!');
    y.push_back('!');
  };

  // x and y borrowed by mutably f, but still
  // available anyway
  std::cout << x << std::endl;
  std::cout << y << std::endl;

  f();

  std::cout << x << std::endl;
  std::cout << y << std::endl;
}
