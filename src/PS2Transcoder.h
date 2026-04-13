#ifndef PS2_TRANSCODER_H
#define PS2_TRANSCODER_H

#include <Arduino.h>
#include "HIDUsage.h"

// Simple fixed-size container for PS/2 byte sequences.
struct PS2Data {
    uint8_t len;
    uint8_t data[16];
};

enum class PS2Modifier : uint8_t {
    NONE = 0x00,
    LCTRL = 0x01,
    LSHIFT = 0x02,
    LALT = 0x04,
    LGUI = 0x08,
    RCTRL = 0x10,
    RSHIFT = 0x20,
    RALT = 0x40,
    RGUI = 0x80
};

class PS2Transcoder {
public:
    PS2Transcoder();

    // Handle a key-down event. Returns a PS2Data with the bytes to send.
    PS2Data onKeyDown(HIDKey key, uint8_t modifiers);

    // Handle a key-up event. Returns a PS2Data with the bytes to send.
    PS2Data onKeyUp(HIDKey key, uint8_t modifiers);

private:
    // Track up to 6 simultaneous non-modifier keys (HID keyboard limit)
    HIDKey _pressedKeys[6];
    uint8_t _pressedCount = 0;
    uint8_t _prevModifiers = 0;

    PS2Data _getMakeCodes(HIDKey key);
    PS2Data _getBreakCodes(HIDKey key);
    PS2Data _updateModifiers(uint8_t newModifiers);
};

// Get PS/2 make code for a HID key. Returns PS2Data with len 0 for unknown keys.
PS2Data getPS2MakeCode(HIDKey key);

// Helper: format bytes into a hex String
String bytesToHexString(const uint8_t *bytes, size_t len);

#endif // PS2_TRANSCODER_H
