#include <array>
#include <cassert>
#include <cstddef>

template <const std::size_t n>
int &first(std::array<int, n> arr) {
  static_assert(
      n >= 1,
      "array needs to have at last size 1!");
  return arr[0];
}
