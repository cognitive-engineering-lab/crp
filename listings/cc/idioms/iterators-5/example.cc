#include <algorithm>
#include <iostream>
#include <vector>

int main() {
  std::vector<int> v{1, 2, 3};

  auto newEnd = remove(v.begin(), v.end(), 2);
  v.erase(newEnd, v.end());

  // Or since C++20
  // std::erase(v, 2);

  for (auto x : v) {
    std::cout << x << std::endl;
  }
}
