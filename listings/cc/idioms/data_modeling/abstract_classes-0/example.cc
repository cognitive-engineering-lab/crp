#include <iostream>
#include <memory>

// Define an abstract class for an interface
struct Shape {
  Shape() = default;
  virtual ~Shape() = default;
  virtual double area() = 0;
};

// Implement the interface for a concrete class
struct Triangle : public Shape {
  double base;
  double height;

  Triangle(double base, double height)
      : base(base), height(height) {}

  double area() override {
    return 0.5 * base * height;
  }
};

// Implement the interface for a concrete class
struct Rectangle : public Shape {
  double width;
  double height;

  Rectangle(double width, double height)
      : width(width), height(height) {}

  double area() override {
    return width * height;
  }
};

// Use an object via a reference to the interface
void printArea(Shape &shape) {
  std::cout << shape.area() << std::endl;
}

int main() {
  Triangle triangle = Triangle{1.0, 1.0};

  printArea(triangle);

  // Use an object via an owned pointer to the
  // interface
  std::unique_ptr<Shape> shape;
  if (true) {
    shape = std::make_unique<Rectangle>(1.0, 1.0);
  } else {
    shape = std::make_unique<Triangle>(
        std::move(triangle));
  }

  // Convert to a reference to the interface
  printArea(*shape);
}
