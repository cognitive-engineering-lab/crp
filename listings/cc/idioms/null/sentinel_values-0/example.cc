#include <memory>

class LargeStructure {
  int field;
  // many fields ...
};

std::unique_ptr<LargeStructure>
parse(char *data, size_t len) {
  // ...

  // on failure
  return nullptr;
}
