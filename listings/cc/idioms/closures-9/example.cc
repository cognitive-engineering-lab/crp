#include <iostream>
#include <string>

int main() {
  std::string x("hello world");
  std::string y("goodnight moon");

  auto f = [&]() {
    std::cout << x << std::endl;
    std::cout << y << std::endl;
  };

  // x and y borrowed by f, but still available
  std::cout << x << std::endl;
  std::cout << y << std::endl;

  f();
}
