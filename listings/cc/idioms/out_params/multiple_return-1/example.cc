#include <tuple>
#include <utility>

std::pair<int, int> get_point() {
  return std::make_pair(5, 6);
}

int main() {
  int x, y;
  std::tie(x, y) = get_point();
  // ...
}
