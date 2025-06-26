#include <cstddef>
#include <vector>

int main() {
  std::vector<int> v{1, 2, 3};
  // undefined behavior!
  int x(v[4]);
}
