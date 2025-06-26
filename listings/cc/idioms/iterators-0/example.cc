#include <iostream>
#include <vector>

int main() {
  std::vector<int> v{1, 2, 3};

  // prints 1, 2, 3
  for (auto &x : v) {
    std::cout << x << std::endl;
    x = x + 1;
  }

  // prints 2, 3, 4
  for (const auto &x : v) {
    std::cout << x << std::endl;
  }
}
