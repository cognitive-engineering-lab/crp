$#include <thread>
$unsigned int cpu_count() {
$    return std::thread::hardware_concurrency();
$}
$
class ThreadPool {
  unsigned int num_threads;

public:
  ThreadPool() : num_threads(cpu_count()) {}
  ThreadPool(unsigned int nt) : num_threads(nt) {}
};

int main() {
  ThreadPool p1;
  ThreadPool p2(4);
}
