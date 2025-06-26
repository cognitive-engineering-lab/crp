#include <algorithm>
#include <vector>

int main() {
  std::vector v{1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto begin = v.begin() + 2;
  auto end = begin + 5;
  bool b(std::any_of(begin, end, [](int n) {
    return n % 2 == 0;
  }));
}
