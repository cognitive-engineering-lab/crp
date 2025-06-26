#include <iostream>
#include <string>

struct Person {
  std::string name;

  operator std::string &() {
    return this->name;
  }
};

void process(const std::string &name) {
  std::cout << name << std::endl;
}

int main() {
  Person alice{"Alice"};

  process(alice);

  return 0;
}
