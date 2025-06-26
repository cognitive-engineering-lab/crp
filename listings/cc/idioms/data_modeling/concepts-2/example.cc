#include <concepts>
#include <iostream>

struct Shape {
  Shape() {}
  virtual ~Shape() {}
  virtual double area() const = 0;
};

template <typename T>
concept shape = std::derived_from<T, Shape>;

struct Triangle : public Shape {
  double base;
  double height;

  Triangle(double base, double height)
      : base(base), height(height) {}

  // still will be used with static dispatch
  double area() const override {
    return 0.5 * base * height;
  }
};

template <shape T>
double twiceArea(const T &shape) {
  return shape.area() * 2;
}

int main() {
  Triangle triangle{1.0, 1.0};

  std::cout << twiceArea(triangle) << std::endl;
  return 0;
}
