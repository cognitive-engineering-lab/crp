#include <optional>
#include <string>

void go(std::optional<std::string> x) {
  std::string a =
      x.or_else([]() {
         return std::make_optional<std::string>();
       }).value();
  // if x was nullopt, then a is ""

  // ...
}
