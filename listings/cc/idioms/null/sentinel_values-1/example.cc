#include <algorithm>

int gcd(int a, int b) {
  if (b == 0 || a == 0) {
    // returns 0 to indicate invalid input
    return 0;
  }

  while (b != 0) {
    int temp = b;
    b = a % b;
    a = temp;
  }
  return std::abs(a);
}
