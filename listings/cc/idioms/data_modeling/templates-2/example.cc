#include <array>
#include <cstddef>

template <size_t N>
std::array<int, N>
makeSequentialArray(int start) {
  std::array<int, N> arr;
  for (size_t i = 0; i < N; i++) {
    arr[i] = start + i;
  }
}
