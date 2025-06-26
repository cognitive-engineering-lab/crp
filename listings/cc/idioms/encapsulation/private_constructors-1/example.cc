#include <iostream>

struct Triangle {
  double base;
  double height;
};

struct Circle {
  double radius;
};

union Shape {
  Triangle triangle;

private:
  Circle circle;

public:
  static Shape make_circle(double radius) {
    Shape s;
    s.circle = Circle(radius);
    return s;
  };
};

int main() {
  Shape triangle;
  triangle.triangle = Triangle{1.0, 2.0};
  Shape circle = Shape::make_circle(1.0);

  // fails to compile
  // circle.circle = Circle{1.0};

  // fails to compile
  // std::cout << shape.circle.radius;
}
