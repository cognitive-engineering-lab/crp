template <totally_ordered T>
T max(const T &x, const T &y) {
  return (x > y) ? x : y;
}

template <>
int max(const int &x, const int &y) {
  return (x > y) ? x + 1 : y + 1;
}
