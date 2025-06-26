#include <iostream>
#include <optional>

std::optional<unsigned int> safe_divide(unsigned int dividend,
                                        unsigned int divisor) {
  if (divisor != 0) {
    return std::optional<unsigned int>(dividend / divisor);
  } else {
    return std::nullopt;
  }
}

void go(unsigned int dividend, unsigned int divisor) {
  if (auto quotient = safe_divide(dividend, divisor)) {
    std::cout << *quotient << std::endl;
  } else {
    std::cout << "Division failed!" << std::endl;
  }
}

int main() {
  go(10, 2);
  go(10, 0);
}
