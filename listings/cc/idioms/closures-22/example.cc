#include <cassert>
#include <functional>

int add(int x, int y) {
  return x + y;
}

int main() {
  using namespace std::placeholders;

  auto addTen = std::bind(add, 10, _1);
  assert(42 == addTen(32));
}
