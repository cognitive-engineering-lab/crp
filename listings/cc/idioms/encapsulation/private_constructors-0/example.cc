#include <string>

struct Person {
  std::string name;
  int age;

private:
  Person() = default;
};

int main() {
  // fails to compile, Person::Person() private
  // Person nobody;

  // fails to compile since C++20
  // Person alice{"Alice", 42};
  return 0;
}
