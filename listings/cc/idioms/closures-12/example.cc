#include <iostream>
#include <string>

int main() {
  std::string x("hello world");
  std::string y("goodnight moon");

  auto f = [x = std::move(x),
            y = std::move(y)]() {
    std::cout << x << std::endl;
    std::cout << y << std::endl;
  };

  // x and y moved into f,
  // empty strings left behind.
  std::cout << x << std::endl;
  std::cout << y << std::endl;

  f();
}
