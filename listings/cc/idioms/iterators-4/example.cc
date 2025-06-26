#include <algorithm>
#include <iostream>
#include <vector>

int main() {
  std::vector v{9, 8, 7, 6, 5, 4, 3, 2, 1};

  for (auto n : v) {
    std::cout << n << ",";
  }
  std::cout << std::endl;

  std::sort(v.begin(), v.end());

  for (auto n : v) {
    std::cout << n << ",";
  }
  std::cout << std::endl;
}
