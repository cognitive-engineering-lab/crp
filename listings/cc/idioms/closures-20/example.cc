#include <string>

int main() {
  int n = 0;

  auto idCounter = [&]<typename T>(T x) {
    n++;
    return x;
  };

  int y = idCounter(5);
  std::string z =
      idCounter.template operator()<std::string>(std::string("hi"));
}
