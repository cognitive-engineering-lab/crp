#include <fstream>

int main() {
  std::ifstream file("/path/to/file");
  if (!file.is_open()) {
    return -1;
  }

  byte buf[1024];
  while (file.good()) {
    file.read(buf, sizeof buf);
    std::streamsize count = file.gcount();

    // use data in buf
  }

  return 0;
}
