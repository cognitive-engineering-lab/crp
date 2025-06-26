#include <iostream>
#include <string>

int main() {
  std::string x("hello world");
  std::string y("goodnight moon");

  auto f = [x = std::move(x), &y]() mutable {
    x.push_back('!');
    std::cout << x << std::endl;
    std::cout << y << std::endl;
  };

  // x moved into f, y borrowed by f
  std::cout << x << std::endl;
  std::cout << y << std::endl;

  f();
}
