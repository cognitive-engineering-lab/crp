#include <iostream>
#include <sstream>
#include <string>

int main() {
  std::string greeting = "hello";
  // Can't write the type of the closure
  auto sayHelloTo = [&](std::string &who) {
    std::ostringstream out;
    out << greeting << " " << who;
    return out.str();
  };

  std::string world("world");
  std::string moon("moon");

  std::cout << sayHelloTo(world) << std::endl;
  std::cout << sayHelloTo(moon) << std::endl;
}
