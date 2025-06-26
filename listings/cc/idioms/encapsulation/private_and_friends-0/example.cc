#include <iostream>
#include <string>

class Person {
  int age;

public:
  std::string name;

  // Because age is private, a public constructor
  // method is needed to create instances.
  Person(std::string name, int age)
      : name(name), age(age) {}

  // Free functions cannot access private members,
  // so this has to be a member function.
  static void example() {
    Person alice{"Alice", 42};
    std::cout << alice.name << std::endl;
    // The private field is visible here, within
    // the class.
    std::cout << alice.age << std::endl;
  }
};

int main() {
  Person alice("Alice", 42);
  std::cout << alice.name << std::endl;
  // compilation error
  // std::cout << alice.age << std::endl;
}
