#include <concepts>

template <typename T>
concept shape = requires(const T &t) {
  { t.area() } -> std::same_as<double>;
};

template <shape T>
double twiceArea(const T &shape) {
  return shape.area() * 2.0;
}
