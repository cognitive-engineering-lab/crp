#include <cstdlib>
#include <cstring>

// widget.h
struct widget_t;
widget_t *alloc_widget();
void free_widget(widget_t *);
void copy_widget(widget_t *dst, widget_t *src);

// widget.cc
class Widget {
  widget_t *widget;

public:
  Widget() : widget(alloc_widget()) {}

  Widget(const Widget &other) : widget(alloc_widget()) {
    copy_widget(widget, other.widget);
  }

  Widget(Widget &&other) : widget(other.widget) {
    other.widget = nullptr;
  }

  ~Widget() {
    free_widget(widget);
  }
};
