#include <utility>

struct Point {
  int x;
  int y;

  operator std::pair<int, int>() const {
    return std::pair(x, y);
  }
};

void process(std::pair<int, int>) {}

int main() {
  Point p1{1, 2};
  Point p2{3, 4};

  std::pair<int, int> xy = p1;
  process(p2);

  return 0;
}
