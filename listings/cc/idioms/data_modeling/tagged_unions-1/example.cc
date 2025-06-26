$#include <iostream>
$
$enum Tag { Rectangle, Triangle, Circle };
$
struct Shape {
$  Tag tag;
$  union {
$    struct {
$      double width;
$      double height;
$    } rectangle;
$    struct {
$      double base;
$      double height;
$    } triangle;
$    struct {
$      double radius;
$    } circle;
$  };
$
  void print_shape() {
    switch (this->tag) {
    case Rectangle: {
      std::cout << "Rectangle" << std::endl;
      break;
    }
    default: {
      std::cout << "Some other shape"
                << std::endl;
      break;
    }
    }
  }
};
