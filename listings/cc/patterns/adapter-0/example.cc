#include <concepts>
#include <iostream>
#include <string>

template <typename T>
concept doubleable = requires(const T t) {
  { t.twice() } -> std::same_as<T>;
};

template <doubleable T>
T quadruple(const T &x) {
  return x.twice().twice();
}

struct DoubleableString {
  std::string str;

  DoubleableString twice() const {
    return DoubleableString{this->str +
                            this->str};
  }
};

int main() {
  auto s = quadruple(
      DoubleableString{std::string("a")});
  std::cout << s.str << std::endl;
}
