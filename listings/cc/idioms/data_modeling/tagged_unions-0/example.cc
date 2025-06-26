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

  double area() {
    switch (this->tag) {
    case Rectangle: {
      return this->rectangle.width *
             this->rectangle.height;
    }
    case Triangle: {
      return 0.5 * this->triangle.base *
             this->triangle.height;
    }
    }
  }
};
