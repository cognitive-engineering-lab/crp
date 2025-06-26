#include <iostream>
#include <memory>

int main() {
  constexpr unsigned int SIZE = 8000000;
  std::unique_ptr b = std::make_unique<
      std::array<unsigned int, SIZE>>();
  for (std::size_t i; i < SIZE; ++i) {
    (*b)[i] = 42;
  }

  // use b so that it isn't optimized away
  for (std::size_t i; i < SIZE; ++i) {
    std::cout << (*b)[i] << std::endl;
  }
}
