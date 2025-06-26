/**
 * @brief Computes the factorial.
 *
 * Computes the factorial in a stack-safe way.
 * The factorial is defined as...
 *
 * @code
 * #include <cassert>
 * #include "factorial.h"
 *
 * int main() {
 *    int res = factorial(3);
 *    assert(6 == res);
 * }
 * @endcode
 *
 * @param n The number of which to take the factorial
 *
 * @return The factorial
 *
 * @exception domain_error If n < 0
 */
int factorial(int n);
