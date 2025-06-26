#include <iostream>

int apply_to_0(auto f) {
  return f(0);
}

int main() {
  int x = 1;
  auto f([=](int n) { return n + x; });
  std::cout << apply_to_0(f) << std::endl;
}
