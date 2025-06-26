#include <string>

int main() {
  auto id = []<typename T>(T x) { return x; };
  int y = id(5);
  std::string z = id(std::string("hi"));
}
