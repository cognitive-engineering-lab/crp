#include <cstring>

int main() {
  char example[6] = "hello";
  char other[6];

  // strncpy takes arguments of type char*
  strncpy(other, example, 6);
}
