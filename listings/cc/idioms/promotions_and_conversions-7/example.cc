#include <cstdint>
#include <iostream>

int main() {
  // Compiles without error (but with a warning).
  uint32_t x = 4294967296;

  // assumes int
  auto y = 1;

  // literal is given a larger type, so it prints
  // correctly
  std::cout << 4294967296 << std::endl;

  // these work as expected
  std::cout << INT64_C(4294967296) << std::endl;

  uint64_t z = INT64_C(4294967296);
  std::cout << z << std::endl;
}
