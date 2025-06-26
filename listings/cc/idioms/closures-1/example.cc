#include <iostream>

int main() {
  int (*f)(int) = [](int n) {
    std::cout << n << std::endl;
    return 2 * n;
  };

  std::cout << f(42) << std::endl;
}
