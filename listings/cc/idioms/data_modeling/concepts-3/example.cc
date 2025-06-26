#include <concepts>

struct Shape {
  Shape() {}
  virtual ~Shape() {}
  virtual double area() = 0;
};

template <typename T>
concept shape = std::derived_from<T, Shape>;

template <shape T>
double twiceArea(const T &shape) {
  // note the call to a method not defined in Shape
  return shape.volume() * 2;
}
