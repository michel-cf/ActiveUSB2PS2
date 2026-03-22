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
