#include <functional>
#include <iostream>

int apply_to_0(std::function<int(int)> f) {
  return f(0);
}

int main() {
  int x = 1;
  // closure is on heap
  auto f(std::function(
      [=](int n) { return n + x; }));
  std::cout << apply_to_0(f) << std::endl;
}
