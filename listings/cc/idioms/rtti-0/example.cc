#include <any>
#include <iostream>
#include <string>

void print_if_string(const std::any &x) {
  try {
    const std::string &s =
        any_cast<std::string const &>(x);
    std::cout << s << std::endl;
  } catch (std::bad_any_cast &e) {
    std::cout << "Not a string!" << std::endl;
  }
}

int main() {
  print_if_string(std::string("hello world"));
  print_if_string(5);
}
