#include <vector>

class Person {
    int age;

public:
    Person() : age(0) {}
}

int main() {
    std::vector<Person> people(3);
    // ...
}
