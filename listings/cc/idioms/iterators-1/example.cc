#include <ranges>
#include <vector>

using namespace std::views;
using namespace std::ranges::views;

int main() {
$  // clang-format off
  // This example requires C++23
  auto v =
    iota(1)
      | filter([](int n) { return n % 2 == 1; })
      | transform([](int n) { return n + 3; })
      | take(10)
      | std::ranges::to<std::vector>();
$  // clang-format on

  // use v...
}
