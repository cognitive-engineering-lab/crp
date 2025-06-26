#include <iostream>
#include <memory>
#include <string>

class Person {
  int age;

  class Passkey {};

public:
  std::string name;

  Person(Passkey, std::string name, int age)
      : name(name), age(age) {}

  static std::unique_ptr<Person>
  createPerson(std::string name, int age) {
    // Other uses of make_unique are not possible
    // because the Passkey type cannot be
    // constructed.
    return std::make_unique<Person>(Passkey(),
                                    name, age);
  }
};
