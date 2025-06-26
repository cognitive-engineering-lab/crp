#include <vector>
#include <stdexcept>

int main() {
    std::vector<int> v;
    // ... populate v ...
    try {
        auto x = v.at(0);
        // use x
    } catch (std::out_of_range &e) {
        // handle error
    }
}
