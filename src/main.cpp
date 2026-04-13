#include <Arduino.h>
#include "CH9350L.h"
#include "PS2Transcoder.h"
#include "HIDUsage.h"

//#if defined(Serial1)
CH9350L ch(Serial1);
//#else
//CH9350L ch(Serial);
//#endif

// PS/2 cable:
// Data: cable: pin 1 (DOUT) - Arduino pin D3 - green wire
// Clock: cable: pin 6 (DCLK) - Arduino pin D2 - light brown wire
// Ground: cable: pin 3 (GND) - Arduino Ground - blue wire

PS2Transcoder ps2Transcoder;

// PS/2 pin assignment (Arduino Nano Every)
static const uint8_t PS2_CLOCK_PIN = 2; // D2
static const uint8_t PS2_DATA_PIN = 3;  // D3

// Release the line (open-collector high)
static inline void ps2_releaseClock() { pinMode(PS2_CLOCK_PIN, INPUT_PULLUP); }
static inline void ps2_releaseData()  { pinMode(PS2_DATA_PIN, INPUT_PULLUP); }
// Drive the line low (open-collector low)
static inline void ps2_driveClockLow() { pinMode(PS2_CLOCK_PIN, OUTPUT); digitalWrite(PS2_CLOCK_PIN, LOW); }
static inline void ps2_driveDataLow()  { pinMode(PS2_DATA_PIN, OUTPUT); digitalWrite(PS2_DATA_PIN, LOW); }

// Generate one clock cycle (fall then rise). Caller must set data before calling.
static void ps2_pulseClock() {
  // Ensure a small settle time
  delayMicroseconds(10);
  // Create falling edge
  ps2_driveClockLow();
  delayMicroseconds(60);
  // Release so it returns high
  ps2_releaseClock();
  delayMicroseconds(60);
}

// Send one PS/2 byte (start, 8 data LSB-first, parity odd, stop)
static void ps2_writeByte(uint8_t b) {
  // start bit (0)
  ps2_driveDataLow();
  ps2_pulseClock();

  // data bits LSB first
  uint8_t parity = 1; // odd parity
  for (uint8_t i = 0; i < 8; ++i) {
    uint8_t bit = (b >> i) & 1;
    if (bit) {
      // release for logical 1
      ps2_releaseData();
      parity ^= 1;
    } else {
      // drive low for logical 0
      ps2_driveDataLow();
    }
    ps2_pulseClock();
  }

  // parity bit
  if (parity) ps2_releaseData(); else ps2_driveDataLow();
  ps2_pulseClock();

  // stop bit (1) - release data
  ps2_releaseData();
  ps2_pulseClock();

  // leave lines released (idle high)
  ps2_releaseData();
  ps2_releaseClock();
  delayMicroseconds(200); // give host time
}

static void ps2_send(const PS2Data &d) {
  for (uint8_t i = 0; i < d.len; ++i) {
    ps2_writeByte(d.data[i]);
  }
}

void printKeyEvent(const char *label, uint8_t keycode, uint8_t mods, const PS2Data &ps2data) {
  Serial.print(label);
  Serial.print(" 0x");
  if (keycode < 16) Serial.print('0');
  Serial.print(keycode, HEX);
  Serial.print(" (");
  Serial.print(keyToString(keycode));
  Serial.print(")");
  Serial.print(" Modifiers: 0x");
  if (mods < 16) Serial.print('0');
  Serial.print(mods, HEX);
  Serial.print(" PS/2: [");
  Serial.print(bytesToHexString(ps2data.data, ps2data.len));
  Serial.println("]");
}

void handleKeyDown(uint8_t keycode, uint8_t modifiers) {
  HIDKey key = static_cast<HIDKey>(keycode);
  PS2Data ps2Bytes = ps2Transcoder.onKeyDown(key, modifiers);
  printKeyEvent("KeyDown:", keycode, modifiers, ps2Bytes);
  // send PS/2 bytes out over D3 (data) / D2 (clock)
  if (ps2Bytes.len) ps2_send(ps2Bytes);
}

void handleKeyUp(uint8_t keycode, uint8_t modifiers) {
  HIDKey key = static_cast<HIDKey>(keycode);
  PS2Data ps2Bytes = ps2Transcoder.onKeyUp(key, modifiers);
  printKeyEvent("KeyUp:", keycode, modifiers, ps2Bytes);
  // send PS/2 bytes out over D3 (data) / D2 (clock)
  if (ps2Bytes.len) ps2_send(ps2Bytes);
}

void handleModifiers(uint8_t mods) {
  if (mods == 0) {
    Serial.println("Mods: none");
    return;
  }
  Serial.print("Mods:");
  if (mods & 0x01) Serial.print(" LCtrl");
  if (mods & 0x02) Serial.print(" LShift");
  if (mods & 0x04) Serial.print(" LAlt");
  if (mods & 0x08) Serial.print(" LGUI");
  if (mods & 0x10) Serial.print(" RCtrl");
  if (mods & 0x20) Serial.print(" RShift");
  if (mods & 0x40) Serial.print(" RAlt");
  if (mods & 0x80) Serial.print(" RGUI");
  Serial.println();
}

void handleMouseRel(uint8_t buttons, int8_t dx, int8_t dy, int8_t wheel) {
  char bbuf[32];
  CH9350L_mouseBtnsToString(buttons, bbuf, sizeof(bbuf));
  char buf[64];
  snprintf(buf, sizeof(buf), "MouseRel btn=%s dx=%d dy=%d wheel=%d", bbuf, dx, dy, wheel);
  Serial.println(buf);
}

void handleMouseAbs(uint8_t id, uint8_t buttons, uint16_t x, uint16_t y, int8_t wheel) {
  char bbuf[32];
  CH9350L_mouseBtnsToString(buttons, bbuf, sizeof(bbuf));
  char buf[96];
  snprintf(buf, sizeof(buf), "MouseAbs id=%u btn=%s x=%u y=%u wheel=%d", (unsigned)id, bbuf, (unsigned)x, (unsigned)y, wheel);
  Serial.println(buf);
}

void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); }

  //#if defined(Serial1)
  ch.begin(38400);
  //#endif
  ch.onKeyDown(handleKeyDown);
  ch.onKeyUp(handleKeyUp);
  ch.onModifiersChanged(handleModifiers);
  ch.onMouseRelative(handleMouseRel);
  ch.onMouseAbsolute(handleMouseAbs);

  // Initialize PS/2 pins (release = idle high)
  ps2_releaseClock();
  ps2_releaseData();

  // Example: set NumLock + CapsLock LEDs (requires CH9350L in working STATE 2)
  // ch.setKeyboardIndicators(CH9350L::LED_NUM | CH9350L::LED_CAPS);

  Serial.println("CH9350L example initialized");
}

void loop() {
  ch.update(); // process incoming UART data from CH9350L

  // Serial-controlled LED test (press keys on serial terminal):
  // '1' = Num, '2' = Caps, '3' = Scroll, '0' = clear
  static uint8_t cur_leds = 0;
  if (Serial.available()) {
    int c = Serial.read();
    if (c == '1') cur_leds ^= CH9350L::LED_NUM;
    else if (c == '2') cur_leds ^= CH9350L::LED_CAPS;
    else if (c == '3') cur_leds ^= CH9350L::LED_SCROLL;
    else if (c == '0') cur_leds = 0;

    if (c == '1' || c == '2' || c == '3' || c == '0') {
      bool ok = ch.setKeyboardIndicators(cur_leds);
      Serial.print("setKeyboardIndicators -> "); Serial.println(ok ? "OK" : "ERR");
      Serial.print("leds=0x"); Serial.println(cur_leds, HEX);
    }
  }

  delay(1);
}
