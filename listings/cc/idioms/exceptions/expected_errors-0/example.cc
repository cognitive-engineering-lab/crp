#include <stdexcept>

double divide(double dividend, double divisor) {
  if (divisor == 0.0) {
    throw std::domain_error("zero divisor");
  }

  return dividend / divisor;
}
