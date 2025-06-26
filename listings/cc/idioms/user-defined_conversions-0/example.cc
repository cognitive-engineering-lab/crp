struct Widget {
  Widget(int) {}
  Widget(int, int) {}
};

void process(Widget w) {}

int main() {
  Widget w1 = 1;
  Widget w2 = {4, 5};
  process(1);
  process({4, 5});

  return 0;
}
