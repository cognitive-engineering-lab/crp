#include <cstddef>
#include <iostream>
#include <string>

struct MyClosure {
  std::string msg;

  std::size_t operator()() {
    std::cout << msg << std::endl;
    return msg.size();
  }
};

int main() {
  MyClosure myClosure{"hello world"};
  myClosure();
}
