#include <cstddef>
#include <vector>

int accessValue(std::vector<std::size_t> indices,
                 std::vector<int> values,
                 std::size_t i) {
  // vector::at throws
  size_t idx(indices.at(i));
  // vector::at throws
  return values.at(idx);
}
