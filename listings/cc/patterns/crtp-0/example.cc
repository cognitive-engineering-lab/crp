#include <iostream>

template <typename T>
struct Shape {
  // This implementation is shared and can call
  // the area method from derived classes without
  // declaring it virtual.
  double twiceArea() {
    return 2.0 * static_cast<T *>(this)->area();
  }
};

struct Triangle : public Shape<Triangle> {
  double base;
  double height;

  Triangle(double base, double height)
      : base(base), height(height) {}

  double area() {
    return 0.5 * base * height;
  }
};

struct Square : public Shape<Square> {
  double side;

  Square(double side) : side(side) {}

  double area() {
    return side * side;
  }
};

int main() {
  Triangle triangle{2.0, 1.0};
  Square square{2.0};

  std::cout << triangle.twiceArea() << std::endl;
  std::cout << square.twiceArea() << std::endl;
}
