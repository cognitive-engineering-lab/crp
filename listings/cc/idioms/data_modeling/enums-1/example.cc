$#include <cstdint>
$
$enum Pin : uint8_t {
$  Pin1 = 0x01,
$  Pin2 = 0x02,
$  Pin3 = 0x04
$};
$
struct InvalidPin {
    uint8_t pin;
};

Pin to_pin(uint8_t pin) {
  // The values are not contiguous, so we can't
  // just check the bounds and then cast.
  switch (pin) {
  case 0x1: { return Pin1; }
  case 0x2: { return Pin2; }
  case 0x4: { return Pin3; }
  }
  throw InvalidPin{pin};
}

int main() {
  try {
    Pin p(to_pin(2));
  } catch (InvalidPin &e) {
    return 0;
  }

  // use pin p
}
