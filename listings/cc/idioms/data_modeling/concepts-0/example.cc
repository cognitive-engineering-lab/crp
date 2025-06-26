#include <iostream>

struct Triangle {
  double base;
  double height;

  Triangle(double base, double height)
      : base(base), height(height) {}

  // NOT virtual: it will be used with static
  // dispatch
  double area() const {
    return 0.5 * base * height;
  }
};

// Generic function using interface
template <class T>
double twiceArea(const T &shape) {
  return shape.area() * 2;
}

int main() {
  Triangle triangle{1.0, 1.0};

  std::cout << twiceArea(triangle) << std::endl;
  return 0;
}
