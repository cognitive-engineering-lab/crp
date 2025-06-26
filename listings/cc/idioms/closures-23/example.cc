#include <iostream>
#include <string>

int main() {
  std::string msg("hello world");
  auto f = [=]() -> const std::string & {
    return msg;
  };
  std::cout << f() << std::endl;
}
