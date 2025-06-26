#include <string>
#include <vector>

template <typename T>
void push_if_even(int n,
                  std::vector<T> &collection,
                  T item) {
  if (n % 2 == 0) {
    collection.push_back(std::move(item));
  }
}

int main() {
  // Operate on the default std::vector
  // implementation
  std::vector<std::string> v{"a", "b"};
  push_if_even(2, v, std::string("c"));

  // Operate on the (likely space-optimized)
  // std::vector implementation
  std::vector<bool> bv{false, true};
  push_if_even(2, bv, false);
}
