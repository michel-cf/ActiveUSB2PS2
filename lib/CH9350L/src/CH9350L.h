#ifndef CH9350L_H
#define CH9350L_H

#include <Arduino.h>
#include <string.h>

class CH9350L {
public:
    using KeyCallback = void(*)(uint8_t keycode, uint8_t modifiers);
    using ModCallback = void(*)(uint8_t modifiers);
    using MouseRelCallback = void(*)(uint8_t buttons, int8_t dx, int8_t dy, int8_t wheel);
    using MouseAbsCallback = void(*)(uint8_t id, uint8_t buttons, uint16_t x, uint16_t y, int8_t wheel);

    // Keyboard LED bits (indicator report bits)
    static constexpr uint8_t LED_NUM   = 0x01;
    static constexpr uint8_t LED_CAPS  = 0x02;
    static constexpr uint8_t LED_SCROLL= 0x04;

    // Set keyboard indicators (Num/Caps/Scroll). NOTE: datasheet documents this
    // command as supported in working *state 2* — it will not take effect while
    // the chip is in state 0 (the default used by the rest of this library).
    // Returns true if the command bytes were written to the UART.
    bool setKeyboardIndicators(uint8_t leds);

    // Require a HardwareSerial instance (no SoftwareSerial/support)
    explicit CH9350L(HardwareSerial &serial);

    // Begin the underlying HardwareSerial at `baud`. Call from setup().
    void begin(unsigned long baud = 115200);
    void update(); // call from main loop to process UART data

    void onKeyDown(KeyCallback cb);
    void onKeyUp(KeyCallback cb);
    void onModifiersChanged(ModCallback cb);
    void onMouseRelative(MouseRelCallback cb);
    void onMouseAbsolute(MouseAbsCallback cb);

    bool sendCommand(const uint8_t *data, size_t len);

private:
    HardwareSerial *_stream = nullptr;

    KeyCallback _keyDownCb = nullptr;
    KeyCallback _keyUpCb = nullptr;
    ModCallback _modCb = nullptr;
    MouseRelCallback _mouseRelCb = nullptr;
    MouseAbsCallback _mouseAbsCb = nullptr;
    // Parser state and buffers for incoming CH9350L frames
    enum ParseState : uint8_t { PS_WAIT_57 = 0, PS_WAIT_AB = 1, PS_READ_TYPE = 2, PS_READ_PAYLOAD = 3, PS_SKIP = 4 };
    ParseState _state = PS_WAIT_57;
    uint8_t _frameType = 0;
    uint8_t _expected = 0;
    uint8_t _payload[128];
    uint8_t _payloadIdx = 0;

    // For keyboard detection (USB standard 8-byte keyboard report)
    uint8_t _prevKeys[6] = {0};
    uint8_t _prevMods = 0;

    void _parseByte(uint8_t b);
};

// Modifier bit masks (USB HID)
enum CH9350L_Mod {
    CH9350L_MOD_LCTRL = 0x01,
    CH9350L_MOD_LSHIFT = 0x02,
    CH9350L_MOD_LALT = 0x04,
    CH9350L_MOD_LGUI = 0x08,
    CH9350L_MOD_RCTRL = 0x10,
    CH9350L_MOD_RSHIFT = 0x20,
    CH9350L_MOD_RALT = 0x40,
    CH9350L_MOD_RGUI = 0x80
};

// Mouse button masks (typical HID mapping)
enum CH9350L_MouseBtn {
    CH9350L_BTN_LEFT = 0x01,
    CH9350L_BTN_RIGHT = 0x02,
    CH9350L_BTN_MIDDLE = 0x04,
    CH9350L_BTN_BACK = 0x08,
    CH9350L_BTN_FORWARD = 0x10
};

// Helper: fill a small buffer with human-readable modifier names.
inline void CH9350L_modsToString(uint8_t mods, char *buf, size_t buflen) {
    if (buflen == 0) return;
    buf[0] = '\0';
    bool first = true;
    auto append = [&](const char *s){
        if (!first) {
            strncat(buf, " ", buflen - strlen(buf) - 1);
        }
        strncat(buf, s, buflen - strlen(buf) - 1);
        first = false;
    };
    if (!mods) { strncpy(buf, "none", buflen); return; }
    if (mods & CH9350L_MOD_LCTRL) append("LCtrl");
    if (mods & CH9350L_MOD_LSHIFT) append("LShift");
    if (mods & CH9350L_MOD_LALT) append("LAlt");
    if (mods & CH9350L_MOD_LGUI) append("LGUI");
    if (mods & CH9350L_MOD_RCTRL) append("RCtrl");
    if (mods & CH9350L_MOD_RSHIFT) append("RShift");
    if (mods & CH9350L_MOD_RALT) append("RAlt");
    if (mods & CH9350L_MOD_RGUI) append("RGUI");
}

// Helper: fill a small buffer with mouse button names
inline void CH9350L_mouseBtnsToString(uint8_t btns, char *buf, size_t buflen) {
    if (buflen == 0) return;
    buf[0] = '\0';
    bool first = true;
    auto append = [&](const char *s){
        if (!first) {
            strncat(buf, "|", buflen - strlen(buf) - 1);
        }
        strncat(buf, s, buflen - strlen(buf) - 1);
        first = false;
    };
    if (!btns) { strncpy(buf, "none", buflen); return; }
    if (btns & CH9350L_BTN_LEFT) append("L");
    if (btns & CH9350L_BTN_RIGHT) append("R");
    if (btns & CH9350L_BTN_MIDDLE) append("M");
    if (btns & CH9350L_BTN_BACK) append("Back");
    if (btns & CH9350L_BTN_FORWARD) append("Fwd");
}

#endif // CH9350L_H