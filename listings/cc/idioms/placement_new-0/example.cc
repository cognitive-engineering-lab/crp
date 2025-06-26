#include <cstddef>
#include <new>

struct LargeWidget {
  std::size_t id;
};

template <typename T>
extern void blackBox(T &x);

void doWork(void *scratch) {
  for (std::size_t i = 0; i < 100; i++) {
    auto *w(new (scratch) LargeWidget{.id = i});
    // use w
    blackBox(w);
    w->~LargeWidget();
  }
}

int main() {
  alignas(alignof(LargeWidget)) char
      memory[sizeof(LargeWidget)];
  void *w = memory;
  doWork(w);
}
