struct Vec2 {
  double x;
  double y;

  Vec2 operator+(const Vec2 &other) const {
    return Vec2{x + other.x, y + other.y};
  }
};

int main() {
  Vec2 a{1.0, 2.0};
  Vec2 b{3.0, 4.0};
  Vec2 c = a + b;
}
