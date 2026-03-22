#include <Arduino.h>
#include "CH9350L.h"

#if defined(USE_SOFTWARE_SERIAL)
#include <SoftwareSerial.h>
// Using SoftwareSerial for boards without multiple hardware UARTs.
// Change pins as needed for your board: RX, TX. Example below uses 10=RX, 11=TX.
// Note: SoftwareSerial has limitations (baud rate, CPU usage) — prefer HardwareSerial when available.
static SoftwareSerial swSerial(10, 11);
// Construct CH9350L with the SoftwareSerial instance.
CH9350L ch(swSerial);
#else
// Default: prefer Serial1 if available, otherwise use Serial.
#if defined(Serial1)
CH9350L ch(Serial1);
#else
CH9350L ch(Serial);
#endif
#endif

/* How to enable the SoftwareSerial example:
 - Option A (in-code): uncomment the following line BEFORE the includes at top of this file:
   #define USE_SOFTWARE_SERIAL

 - Option B (PlatformIO build flag): add this to your `platformio.ini` under the environment:
   build_flags = -DUSE_SOFTWARE_SERIAL

 Then adjust the SoftwareSerial RX/TX pins above to match your wiring.
*/

void printKeyEvent(const char *label, uint8_t keycode, uint8_t mods) {
  char buf[64];
  sprintf(buf, "%s 0x%02X mods=0x%02X", label, keycode, mods);
  Serial.println(buf);
}

void handleKeyDown(uint8_t keycode, uint8_t modifiers) {
  printKeyEvent("KeyDown:", keycode, modifiers);
}

void handleKeyUp(uint8_t keycode, uint8_t modifiers) {
  printKeyEvent("KeyUp:", keycode, modifiers);
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
  Serial.printf("MouseRel btn=%s dx=%d dy=%d wheel=%d\n", bbuf, dx, dy, wheel);
}

void handleMouseAbs(uint8_t id, uint8_t buttons, uint16_t x, uint16_t y, int8_t wheel) {
  char bbuf[32];
  CH9350L_mouseBtnsToString(buttons, bbuf, sizeof(bbuf));
  Serial.printf("MouseAbs id=%d btn=%s x=%d y=%d wheel=%d\n", id, bbuf, x, y, wheel);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  ch.begin(115200);
  ch.onKeyDown(handleKeyDown);
  ch.onKeyUp(handleKeyUp);
  ch.onModifiersChanged(handleModifiers);
  ch.onMouseRelative(handleMouseRel);
  ch.onMouseAbsolute(handleMouseAbs);

  Serial.println("CH9350L example initialized");
}

void loop() {
  ch.update(); // process incoming UART data from CH9350L
  delay(1);
}
