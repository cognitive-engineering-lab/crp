int main() {
  // Local variables are lvalues,
  int x(0);
  // and therefore may be assigned to.
  x = 42;

  // x is converted to an rvalue when needed.
  int y = x + 1;
}
