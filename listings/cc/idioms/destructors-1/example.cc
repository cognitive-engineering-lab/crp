#include <iostream>
#include <string>

struct Part {
  std::string name;

  ~Part() {
    std::cout << "Dropped " << name << std::endl;
  }
};

struct Widget {
  Part part1;
  Part part2;
  Part part3;
};

int main() {
  Widget w{"1", "2", "3"};
  // Prints:
  // 3
  // 2
  // 1
}
