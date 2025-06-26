#include <ranges>
#include <iostream>

using namespace std::views;
using namespace std::ranges::views;

int main() {
$  // clang-format off
    for (auto x :
        iota(1)
          | filter([](int n) { return n % 2 == 1; })
          | transform([](int n) { return n + 3; })
          | take(10)) {
      std::cout<< x << std::endl;
    }
$  // clang-format on
}
