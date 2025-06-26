class Base {
  int x;

public:
  Base() : x(0) {}
};

class Derived : Base {
public:
  // Calls the default constructor for Base
  Derived() {}
};
