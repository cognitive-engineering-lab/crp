int twice(int n) {
  return n * n;
}

struct MyPair {
  int x;
  int y;

  MyPair(int x, int y) : x(x), y(y) {}

  static MyPair make() {
    return MyPair{0, 0};
  }
};

int main() {
  // convert a function to a function pointer
  int (*twicePtr)(int) = twice;
  int result = twicePtr(5);

  // Per C++23 11.4.5.1.6, can't take the address
  // of a constructor.
  // MyPair (*ctor)(int, int) = MyPair::MyPair;
  // MyPair pair = ctor(10, 20);

  // convert a static method to a function
  // pointer
  MyPair (*methodPtr)() = MyPair::make;
  MyPair pair2 = methodPtr();

  // convert a non-capturing closure to a
  // function pointer
  int (*closure)(int) = [](int x) -> int {
    return x * 5;
  };
  int closureRes = closure(2);
}
