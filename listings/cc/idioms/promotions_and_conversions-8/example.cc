struct Wire {
  bool ready;
  unsigned int value;

  explicit operator bool() const { return ready; }
};

int main() {
  Wire w{false, 0};
  // ...

  if (w) {
    // use w.value
  } else {
    // do something else
  }
}
