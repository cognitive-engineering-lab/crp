#include <iostream>

int process(int n) {
  std::cout << n << std::endl;
  return 2 * n;
}

int main() {
  auto f = process;
  // or with type annotation
  // int (*f)(int) = process;

  std::cout << f(42) << std::endl;
}
