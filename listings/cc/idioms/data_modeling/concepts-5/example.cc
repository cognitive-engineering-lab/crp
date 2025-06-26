template <typename T>
  requires shape<T>
double twiceArea(const T &shape) {
  return 2.0 * shape.area();
}
