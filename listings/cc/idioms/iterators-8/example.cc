#include <iostream>
$#include <memory>
$#include <vector>

template <typename V>
class Tree {
public:
$  class iterator {
$    std::vector<const Tree<V> *> rest;
$
$  public:
$    using difference_type = long;
$    using value_type = V;
$    using pointer = const V *;
$    using reference = const V &;
$    using iterator_category =
$        std::forward_iterator_tag;
$
$    iterator() {}
$    iterator(const Tree<V> *start) {
$      rest.push_back(start);
$    }
$
$    reference operator*() const {
$      return rest.back()->value;
$    }
$
$    iterator &operator++() {
$      const Tree<V> *t = rest.back();
$      rest.pop_back();
$      if (t->right) {
$        rest.push_back(t->right.get());
$      }
$      if (t->left) {
$        rest.push_back(t->left.get());
$      }
$      return *this;
$    }
$
$    iterator operator++(int) {
$      iterator retval = *this;
$      const Tree<V> *t = rest.back();
$      rest.pop_back();
$      if (t->right) {
$        rest.push_back(t->right.get());
$      }
$      if (t->left) {
$        rest.push_back(t->left.get());
$      }
$      return retval;
$    }
$
$    bool operator==(const iterator &other) const {
$      return rest == other.rest;
$    }
$
$    bool operator!=(const iterator &other) const {
$      return !(*this == other);
$    }
$  };
$
  iterator begin() const {
    return iterator(this);
  }

  iterator end() const {
    return iterator();
  }
$
$  V value;
$  std::unique_ptr<Tree<V>> left;
$  std::unique_ptr<Tree<V>> right;
};

int main() {
  Tree<int> t{1,
              std::make_unique<Tree<int>>(
                  2, nullptr, nullptr),
              std::make_unique<Tree<int>>(
                  3,
                  std::make_unique<Tree<int>>(
                      4, nullptr, nullptr),
                  nullptr)};

  for (auto v : t) {
    std::cout << v << std::endl;
  }
}
