$struct Event {
$  virtual ~Event() = default;
$};
$
$struct ClickEvent : public Event {
$  int x;
$  int y;
$};
$
$struct ResizeEvent : public Event {
$  int old_height;
$  int old_width;
$  int new_height;
$  int new_width;
$};
$
struct DoSomething : public Event {
  double how_much;
}

struct DoSomethingElse : public Event {
  double how_many;
}

void handle_event(Event *e) {
$  if (auto click_event =
$          dynamic_cast<ClickEvent *>(e)) {
$    // ...
$  } else if (auto resize_event =
$                 dynamic_cast<ResizeEvent *>(e)) {
$    // ...
  // ...
  } else if (auto user_event =
                 dynamic_cast<DoSomething *>(e)) {
    // ...
  } else if (auto user_event =
                 dynamic_cast<DoSomethingElse *>(
                     e)) {
    // ...
  } else {
    // ... handle unknown event ...
  }
}
