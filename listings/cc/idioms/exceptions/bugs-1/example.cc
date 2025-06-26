#include <cassert>
#include <cstddef>

template <typename T>
class Widget {
  T *parts;
  std::size_t partCount;

public:
  // ... constructors ...

  /**
   * @pre n must be smaller than partCount
   */
  T getPart(std::size_t n) {
    // Unlike in Rust, this can be disabled,
    // e.g., with -DNDEBUG.
    assert(n < partCount);
    return *(parts + n);
  }
};
