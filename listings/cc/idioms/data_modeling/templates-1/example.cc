#include <optional>
$#include <stdexcept>
$#include <vector>

template <typename Label>
class DirectedGraph {
$  std::vector<std::vector<size_t>> adjacencies;
$  std::vector<Label> nodeLabels;
$
public:
$  size_t addNode(Label label) {
$    adjacencies.push_back(std::vector<size_t>());
$    nodeLabels.push_back(std::move(label));
$    return numNodes() - 1;
$  }
$
$  void addEdge(size_t from, size_t to) {
$    size_t numNodes = this->numNodes();
$    if (from >= numNodes || to >= numNodes) {
$      throw std::invalid_argument(
$          "Node index out of range");
$    }
$    adjacencies[from].push_back(to);
$  }
$
$  size_t numNodes() const {
$    return adjacencies.size();
$  }
$
  std::optional<size_t> smallestNode() {
    if (nodeLabels.empty()) {
      return std::nullopt;
    }
    Label &least = nodeLabels[0];
    size_t index = 0;

    for (int i = 1; i < nodeLabels.size(); i++) {
      if (least > nodeLabels[i]) {
        least = nodeLabels[i];
        index = i;
      }
    }
    return std::optional(index);
  }
};
