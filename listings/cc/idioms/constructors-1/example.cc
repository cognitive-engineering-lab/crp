#include <iostream>
#include <stdexcept>

class ThreadPool {
  unsigned int num_threads;

public:
  ThreadPool(unsigned int nt) : num_threads(nt) {
    if (num_threads == 0) {
      throw std::domain_error(
          "Cannot have zero threads");
    }
  }
};

int main() {
  try {
    ThreadPool p(0);
    // use p here
  } catch (const std::domain_error &e) {
    std::cout << e.what() << std::endl;
  }
}
