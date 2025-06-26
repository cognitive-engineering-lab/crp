#include <memory>

class BinaryTree {
  // This needs to be an inner class in order for
  // it to be private.
  class TreeNode {
    friend class BinaryTree;

    int value;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;

  public:
    TreeNode(int value)
        : value(value), left(nullptr),
          right(nullptr) {}

  private:
    static void
    insert(std::unique_ptr<TreeNode> &node,
           int value) {
      if (node) {
        node->insert(value);
      } else {
        node = std::make_unique<TreeNode>(value);
      }
    }

    void insert(int value) {
      if (value < this->value) {
        insert(this->left, value);
      } else {
        insert(this->right, value);
      }
    }
  };

  std::unique_ptr<TreeNode> root;

public:
  BinaryTree() : root(nullptr) {}

  void insert(int value) {
    TreeNode::insert(root, value);
  }
};

int main() {
  BinaryTree b;
  b.insert(42);

  return 0;
}
