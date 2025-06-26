#include <cstdint>

enum Pin : uint8_t {
  Pin1 = 0x01,
  Pin2 = 0x02,
  Pin3 = 0x04
};

enum Mode : uint8_t {
  Output = 0x03,
  Pullup = 0x04,
  Analog = 0x27
  // ...
};

void low_level_set_pin(uint8_t pin, uint8_t mode);

void set_pin_mode(Pin pin, Mode mode) {
  low_level_set_pin(pin, mode);
}
