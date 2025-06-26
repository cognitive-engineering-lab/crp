#include <iostream>
#include <string>

int main() {
  std::string x("hello world");
  std::string y("goodnight moon");

  auto f = [=]() mutable {
    x.push_back('!');
    y.push_back('!');
  };

  // copies of x and y owned by f, originals
  // still available
  std::cout << x << std::endl;
  std::cout << y << std::endl;

  f();

  // still don't have the !, since the copies
  // were modified not the originals
  std::cout << x << std::endl;
  std::cout << y << std::endl;
}
