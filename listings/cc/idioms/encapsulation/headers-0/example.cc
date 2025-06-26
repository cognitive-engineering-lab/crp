// person.h
class Person {
  std::string name;

public:
  Person(std::string name) : name(name) {}
  const std::string &getName();
};

// person.cc
#include <string>
#include "person.h"

const std::string &Person::getName() {
  return this->name;
}

// client.cc
#include <string>
#include "person.h"

int main() {
  Person p("Alice");
  const std::string &name = p.getName();

  // ...
}
