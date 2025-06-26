$#include <memory>
$#include <string>
$
struct Age {
  unsigned int years;

  Age(unsigned int years) : years(years) {}

  // copy and move constructors and destructor
  // implicitly declared and defined
};

struct Person {
  Age age;
  std::string name;
  std::shared_ptr<Person> best_friend;

  Person(Age age,
         std::string name,
         std::shared_ptr<Person> best_friend)
      : age(std::move(age)), name(std::move(name)),
        best_friend(std::move(best_friend)) {}

  // copy and move constructors and destructor
  // implicitly declared and defined
};
