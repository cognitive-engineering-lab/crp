unsigned int shift(unsigned int x,
                   unsigned int shiftAmount) {
  return x << shiftAmount;
}

unsigned int shift(unsigned int x) {
  return shift(x, 2);
}

int main() {
  unsigned int a = shift(7); // shifts by 2
}
