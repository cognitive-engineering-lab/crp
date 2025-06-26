decltype(auto) makeClosure(int n) {
  return [n]() { return [n]() { return n; }; };
}
