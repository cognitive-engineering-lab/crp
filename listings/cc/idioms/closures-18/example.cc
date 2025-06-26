#include <iostream>

decltype(auto) makeConst(int n) {
  return [n]() { return n; };
}

int main() {
  auto f = makeConst(42);
  std::cout << f() << std::endl;
}
