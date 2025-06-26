#include <stdexcept>
#include <vector>

template <typename Label>
class DirectedGraph {
  std::vector<std::vector<size_t>> adjacencies;
  // The mistake is here: size_t should be Label
  std::vector<size_t> nodeLabels;

public:
  Label getNode(size_t nodeId) {
    return nodeLabels[nodeId];
  }

  size_t addNode(Label label) {
    adjacencies.push_back(std::vector<size_t>());
    nodeLabels.push_back(std::move(label));
    return numNodes() - 1;
  }

  size_t numNodes() const {
    return adjacencies.size();
  }
};

#define BOOST_TEST_MODULE DirectedGraphTests
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_add_node_int) {
  DirectedGraph<int> g;
  auto n1 = g.addNode(1);
  BOOST_CHECK_EQUAL(1, g.getNode(n1));
}

BOOST_AUTO_TEST_CASE(test_add_node_float) {
  DirectedGraph<float> g;
  float label = 1.0f;
  auto n1 = g.addNode(label);
  BOOST_CHECK_CLOSE(label, g.getNode(n1), 0.0001);
}
