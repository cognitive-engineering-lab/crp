#include <iostream>

bool safe_divide(unsigned int dividend,
                 unsigned int divisor,
                 unsigned int &quotient) {
  if (divisor != 0) {
    quotient = dividend / divisor;
    return true;
  } else {
    return false;
  }
}

void go(unsigned int dividend,
        unsigned int divisor) {
  unsigned int quotient;
  if (safe_divide(dividend, divisor, quotient)) {
    std::cout << quotient << std::endl;
  } else {
    std::cout << "Division failed!" << std::endl;
  }
}

int main() {
  go(10, 2);
  go(10, 0);
}
