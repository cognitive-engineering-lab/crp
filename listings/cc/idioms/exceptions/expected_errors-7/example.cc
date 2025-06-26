#include <exception>

struct ErrorA : public std::exception {
  const char *msg = "ErrorA was produced";
  const char *what() const noexcept override {
    return msg;
  }
};

void mightThrowA() {}

struct ErrorB : public std::exception {
  const char *msg = "ErrorA was produced";
  const char *what() const noexcept override {
    return msg;
  }
};

void mightThrowB() {}

void process() {
  mightThrowA();
  mightThrowB();
}
