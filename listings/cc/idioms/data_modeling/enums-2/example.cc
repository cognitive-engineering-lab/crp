#include <cstdint>

// Actual enum
enum PinImpl : uint8_t {
  Pin1 = 0x01,
  Pin2 = 0x02,
  Pin3 = 0x04
};

class LastPin{};

// Wrapper type
struct Pin {
  PinImpl pin;

  // Conversion constructor so that PinImpl can be
  // used as a Pin.
  Pin(PinImpl p) : pin(p) {}

  // Conversion method so wrapper type can be
  // used with switch statement.
  operator PinImpl() {
    return this->pin;
  }

  Pin next() const {
    switch (pin) {
    case Pin1:
      return Pin(Pin2);
    case Pin2:
      return Pin(Pin3);
    default:
      throw LastPin{};
    }
  }
};
