#include <variant>

struct Rectangle {
  double width;
  double height;
};

struct Triangle {
  double base;
  double height;
};

using Shape = std::variant<Rectangle, Triangle>;

double area(const Shape &shape) {
  return std::visit(
      [](auto &&arg) -> double {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Rectangle>) {
          return arg.width * arg.height;
        }
      },
      shape);
}
