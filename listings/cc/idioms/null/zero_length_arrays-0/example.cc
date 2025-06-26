#include <cstddef>
#include <cassert>

int c_style_sum(std::size_t len, int arr[]) {
    int sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum;
}

int main() {
    int sum = c_style_sum(0, nullptr);
    assert(sum == 0);
}
