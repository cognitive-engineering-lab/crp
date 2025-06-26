#include <stdexcept>
#include <string>

class NonEmpty {
  std::string s;

public:
  NonEmpty(std::string s) : s(s) {
    if (this->s.empty()) {
      throw std::domain_error("empty string");
    }
  }
};

int main() {
  std::string s("");
  NonEmpty x = s; // throws

  return 0;
}
