#include <stdexcept>

int produce_42() {
  return 42;
}

int fail() {
  throw std::runtime_error("oops");
}

int useCallback(int (*func)(void)) {
  return func();
}

int main() {
  try {
    int x = useCallback(produce_42);
    int y = useCallback(fail);

    // use x and y
  } catch (std::runtime_error &e) {
    // handle error
  }
}
