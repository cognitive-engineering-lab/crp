#include <ranges>
#include <vector>

// Uses the same Shape definition.
enum Tag { Rectangle, Triangle };

struct Shape {
  Tag tag;
  union {
    struct {
      double width;
      double height;
    } rectangle;
    struct {
      double base;
      double height;
    } triangle;
  };
};

std::vector<Shape> get_shapes() {
  return std::vector<Shape>{
      Shape{Triangle, {.triangle = {1.0, 1.0}}},
      Shape{Triangle, {.triangle = {1.0, 1.0}}},
      Shape{Rectangle, {.rectangle = {1.0, 1.0}}},
  };
}

std::vector<Shape> get_shapes();

int main() {
  std::vector<Shape> shapes = get_shapes();

  auto is_triangle = [](Shape shape) {
    return shape.tag == Triangle;
  };

  // Create an iterator that only sees the
  // triangles. (std::views::filter is from C++20,
  // but the same effect can be acheived with a
  // custom iterator.)
  auto triangles =
      shapes | std::views::filter(is_triangle);

  double total_base = 0.0;
  for (auto &triangle : triangles) {
    // Skip checking the tag because we know we
    // have only triangles.
    total_base += triangle.triangle.base;
  }

  return 0;
}
