$#include <memory>
$
$struct Shape {
$  Shape() {}
$  virtual ~Shape() {}
$  virtual double area() = 0;
$};
$
template<typename S>
void store(S s, std::unique_ptr<Shape> data) {
    // Will pointers or references in `s` become dangling while `data`
    // is still in use?
	*data = s;
}
