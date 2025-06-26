// Using Boost.Test
// https://www.boost.org/doc/libs/1_84_0/libs/test/doc/html/index.html
#include <string>

class Person {
public:
  std::string name;

private:
  int age;

  friend class PersonTest;

public:
  Person(std::string name, int age)
      : name(name), age(age) {}

  void have_birthday() {
    this->age = this->age + 1;
  }
};

#define BOOST_TEST_MODULE PersonTestModule
#include <boost/test/included/unit_test.hpp>

class PersonTest {
public:
  static void test_have_birthday() {
    Person alice("Alice", 42);
    BOOST_CHECK_EQUAL(alice.age, 42);

    alice.have_birthday();
    BOOST_CHECK_EQUAL(alice.age, 43);
  }
};

BOOST_AUTO_TEST_CASE(have_birthday_test) {
  PersonTest::test_have_birthday();
}
