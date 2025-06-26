#include <memory>

template <typename V>
class Tree {
public:
    V value;
    std::unique_ptr<Tree<V>> left;
    std::unique_ptr<Tree<V>> right;
};
