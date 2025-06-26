#include <expected>
#include <string>

int main() {
  std::expected<int, std::string> res(42);
  auto x(res.transform([](int n) { return n * 2; }));
}
