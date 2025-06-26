#include <iostream>
#include <string>

void display(const std::string &msg) {
  std::cout << "Displaying: " << msg << std::endl;
}

int main() {
  // no const qualifier
  std::string message("hello world");

  // used where const expected
  display(message);
}
