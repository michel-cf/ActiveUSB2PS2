CH9350L Arduino Library
=======================

This library provides basic support for the CH9350L USB HID <-> UART bridge chip. It parses CH9350L framed reports and exposes callbacks for key down, key up and modifier changes.

Usage
-----

Include the header and construct a `CH9350L` with a `Stream` (e.g. `Serial`, `Serial1`, or `SoftwareSerial`):

```cpp
#include "CH9350L.h"

// HardwareSerial example
CH9350L ch(Serial1);

// SoftwareSerial example (define USE_SOFTWARE_SERIAL to enable in example)
// #include <SoftwareSerial.h>
// static SoftwareSerial sw(10, 11);
// CH9350L ch(sw);

void setup() {
  Serial.begin(115200);
  ch.begin(115200);
  ch.onKeyDown([](uint8_t k,uint8_t m){ Serial.printf("KD 0x%02X mods=0x%02X\n", k, m); });
  ch.onKeyUp([](uint8_t k,uint8_t m){ Serial.printf("KU 0x%02X mods=0x%02X\n", k, m); });
  ch.onModifiersChanged([](uint8_t mods){ Serial.printf("Mods 0x%02X\n", mods); });
}

void loop() {
  ch.update();
}
```

SoftwareSerial notes
--------------------

- To enable the `SoftwareSerial` example in the sketch, either add `#define USE_SOFTWARE_SERIAL` before the includes in `src/main.cpp` or add this build flag to `platformio.ini`:

```
[env:uno]
build_flags = -DUSE_SOFTWARE_SERIAL
```

- `SoftwareSerial` has limitations (baud rate, CPU usage). Prefer hardware UART when possible.

Building the project (PlatformIO)
--------------------------------

If you have PlatformIO CLI installed globally:

```bash
platformio run -e uno
```

If PlatformIO is not installed system-wide, create and use a local Python virtualenv (example used during development):

```bash
python3 -m venv .venv
.venv/bin/python -m pip install --upgrade pip setuptools wheel
.venv/bin/pip install platformio
.venv/bin/python -m platformio run -e uno
```

Library structure
-----------------

- `lib/CH9350L/src/CH9350L.h` — public header
- `lib/CH9350L/src/CH9350L.cpp` — implementation
- `lib/CH9350L/library.json` — library manifest

Extending
---------

The current parser implements keyboard reports and modifier change callbacks. If you want mouse events (relative/absolute) or handling of other CH9350L frames (status, device connection), open an issue or request specific frame handling.

Hardware usage
--------------

- Required mode: the CH9350L must be configured to operate as a UART (serial) HID bridge — not in native USB-host mode. Your module's `SEL`/`STATE` selection determines this; set the selector to the UART position according to your module's silkscreen or documentation so the chip exposes the UART interface.

- Baud rates: the library works with standard UART baud rates. Choose one of these common rates and use the same value both for the Arduino `Serial` and for `ch.begin()`:

  - 9600   -> `Serial.begin(9600); ch.begin(9600);`
  - 19200  -> `Serial.begin(19200); ch.begin(19200);`
  - 38400  -> `Serial.begin(38400); ch.begin(38400);`
  - 57600  -> `Serial.begin(57600); ch.begin(57600);`
  - 115200 -> `Serial.begin(115200); ch.begin(115200);` (recommended)

  Example (HardwareSerial):

  ```cpp
  Serial1.begin(115200);
  CH9350L ch(Serial1);
  ch.begin(115200);
  ```

- Notes about `SEL` / state: breakout boards vary. Some boards use a solder jumper, DIP switch, or a pin labeled `SEL` or `STATE` to select between USB and UART operation. For reliable operation with this library, ensure the board is set to the UART position (consult your board's documentation or the CH9350L datasheet for the exact selector polarity).

SEL and STATE (datasheet details)
--------------------------------

These notes are taken from the official CH9350 datasheet included in this repository and describe the chip pin behavior you should configure for UART operation:

 - `SEL` (chip Pin 45): the working-mode selection pin read at power-on/reset. For this library and typical keyboard/mouse usage, the CH9350L must be in Lower Computer mode so it can be connected to a USB device (keyboard/mouse) and forward HID events over UART. Ensure `SEL` is HIGH at power-on/reset (the chip has a default pull-up) to select Lower Computer mode.

 - Working state `S0` and `S1` (chip Pin 4 and 5): this library expects the CH9350L to be in working *state 0* so it reports raw keyboard/mouse events over UART. The working-state selector signals are `S0` and `S1` — on the CH9350L IC these correspond to chip pins 4 and 5.Ensure `S0` and `S1` are BOTH HIGH (logical 1) at power-on/reset (the chip has a default pull-up) so the chip enters state 0.

- Baud rate configuration pins: `BAUD0` (Pin 38) and `BAUD1` (Pin 37). These pins default to input pull-up (so the default baud is selected automatically). The supported baud rates and pin states from the datasheet are:

  - `BAUD0 = HIGH`, `BAUD1 = HIGH`  -> 115200 bps (default)
  - `BAUD0 = HIGH`, `BAUD1 = LOW`   -> 57600 bps
  - `BAUD0 = LOW`,  `BAUD1 = HIGH`  -> 38400 bps
  - `BAUD0 = LOW`,  `BAUD1 = LOW`   -> 300000 bps

  Use the corresponding logic level on the BAUD pins (or set the board jumpers) before power-on so the chip selects the desired UART rate. The datasheet recommends using the default 115200 bps.

UART characteristics
--------------------

- Default UART parameters: 115200 bps, 8 data bits, 1 stop bit, no parity (8N1).
- Output level is 3.3V (5V-tolerant interface is commonly provided by many breakouts). Observe BAUD pin voltage limits in the datasheet.

Wiring examples (115200 and lower)
----------------------------------

The tables and examples below show how to set `BAUD0` (Pin 38), `BAUD1` (Pin 37) and `SEL` (Pin 45) for common UART rates. Pins are referenced at power-on/reset — change the jumper/solder pads while power is off and then apply power.

  - Notes:
  - Use 3.3V logic for these pins (chip input limit is 3.6V). Many breakouts provide level-shifted headers or are 5V-tolerant on UART pins, but check your board's documentation.
  - If the BAUD pins are left floating they default to pull-ups (115200 bps).
  - Working-state pins `S0` (pin 4) and `S1` (pin 5) must both be HIGH at power-on/reset so the chip enters working *state 0* used by this library. Leave `S0`/`S1` pulled-up or tie to VCC (3.3V) — do not drive them above 3.6V.

Pin wiring (examples)

1) 115200 bps (default)

  - BAUD0: HIGH (pull-up / leave open if breakout has pull-up)
  - BAUD1: HIGH (pull-up / leave open if breakout has pull-up)

  Example code:

  ```cpp
  Serial1.begin(115200);
  CH9350L ch(Serial1);
  ch.begin(115200);
  ```

2) 57600 bps

  - BAUD0: HIGH
  - BAUD1: LOW
  
  Example code: use `Serial1.begin(57600); ch.begin(57600);`

3) 38400 bps

  - BAUD0: LOW
  - BAUD1: HIGH
  
  Example code: use `Serial1.begin(38400); ch.begin(38400);`

Power-cycle requirement
-----------------------

After changing the BAUD, power-cycle the module so the CH9350L reads the pin states at reset and selects the configured mode/baud.
