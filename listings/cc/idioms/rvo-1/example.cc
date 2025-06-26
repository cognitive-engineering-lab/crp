#include <array>

std::array<int, 10> make() {
    std::array<int, 10> v;
    for (int i = 0; i < 10; i++) {
        v[i] = i;
    }
    return v;
}
