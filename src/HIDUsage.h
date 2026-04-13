#ifndef HID_USAGE_H
#define HID_USAGE_H

#include <Arduino.h>
#include <stdint.h>


enum class HIDKey : uint8_t {
    NONE = 0x00,
    ERROR_ROLLOVER = 0x01,
    POST_FAIL = 0x02,
    ERROR_UNDEFINED = 0x03,
    A = 0x04,
    B = 0x05,
    C = 0x06,
    D = 0x07,
    E = 0x08,
    F = 0x09,
    G = 0x0A,
    H = 0x0B,
    I = 0x0C,
    J = 0x0D,
    K = 0x0E,
    L = 0x0F,
    M = 0x10,
    N = 0x11,
    O = 0x12,
    P = 0x13,
    Q = 0x14,
    R = 0x15,
    S = 0x16,
    T = 0x17,
    U = 0x18,
    V = 0x19,
    W = 0x1A,
    X = 0x1B,
    Y = 0x1C,
    Z = 0x1D,
    NUM_1 = 0x1E,
    NUM_2 = 0x1F,
    NUM_3 = 0x20,
    NUM_4 = 0x21,
    NUM_5 = 0x22,
    NUM_6 = 0x23,
    NUM_7 = 0x24,
    NUM_8 = 0x25,
    NUM_9 = 0x26,
    NUM_0 = 0x27,
    ENTER = 0x28,
    ESC = 0x29,
    BACKSPACE = 0x2A,
    TAB = 0x2B,
    SPACE = 0x2C,
    MINUS = 0x2D,
    EQUAL = 0x2E,
    LEFT_BRACE = 0x2F,
    RIGHT_BRACE = 0x30,
    BACKSLASH = 0x31,
    NON_US_HASH = 0x32,
    SEMICOLON = 0x33,
    APOSTROPHE = 0x34,
    GRAVE = 0x35,
    COMMA = 0x36,
    DOT = 0x37,
    SLASH = 0x38,
    CAPS_LOCK = 0x39,
    F1 = 0x3A,
    F2 = 0x3B,
    F3 = 0x3C,
    F4 = 0x3D,
    F5 = 0x3E,
    F6 = 0x3F,
    F7 = 0x40,
    F8 = 0x41,
    F9 = 0x42,
    F10 = 0x43,
    F11 = 0x44,
    F12 = 0x45,
    PRINTSCREEN = 0x46,
    SCROLL_LOCK = 0x47,
    PAUSE = 0x48,
    INSERT = 0x49,
    HOME = 0x4A,
    PAGE_UP = 0x4B,
    DELETE = 0x4C,
    END = 0x4D,
    PAGE_DOWN = 0x4E,
    RIGHT = 0x4F,
    LEFT = 0x50,
    DOWN = 0x51,
    UP = 0x52,
    NUM_LOCK = 0x53,
    KP_SLASH = 0x54,
    KP_ASTERISK = 0x55,
    KP_MINUS = 0x56,
    KP_PLUS = 0x57,
    KP_ENTER = 0x58,
    KP_1 = 0x59,
    KP_2 = 0x5A,
    KP_3 = 0x5B,
    KP_4 = 0x5C,
    KP_5 = 0x5D,
    KP_6 = 0x5E,
    KP_7 = 0x5F,
    KP_8 = 0x60,
    KP_9 = 0x61,
    KP_0 = 0x62,
    KP_DOT = 0x63,
    NON_US_BACKSLASH = 0x64,
    APPLICATION = 0x65,
    POWER = 0x66,
    KP_EQUAL = 0x67,
    F13 = 0x68,
    F14 = 0x69,
    F15 = 0x6A,
    F16 = 0x6B,
    F17 = 0x6C,
    F18 = 0x6D,
    F19 = 0x6E,
    F20 = 0x6F,
    F21 = 0x70,
    F22 = 0x71,
    F23 = 0x72,
    F24 = 0x73,
    // Add other keys as needed
};

inline const char* keyToString(uint8_t keycode) {
    switch (static_cast<HIDKey>(keycode)) {
        case HIDKey::NONE: return "None";
        case HIDKey::ERROR_ROLLOVER: return "ErrorRollover";
        case HIDKey::POST_FAIL: return "POSTFail";
        case HIDKey::ERROR_UNDEFINED: return "ErrorUndefined";
        case HIDKey::A: return "A";
        case HIDKey::B: return "B";
        case HIDKey::C: return "C";
        case HIDKey::D: return "D";
        case HIDKey::E: return "E";
        case HIDKey::F: return "F";
        case HIDKey::G: return "G";
        case HIDKey::H: return "H";
        case HIDKey::I: return "I";
        case HIDKey::J: return "J";
        case HIDKey::K: return "K";
        case HIDKey::L: return "L";
        case HIDKey::M: return "M";
        case HIDKey::N: return "N";
        case HIDKey::O: return "O";
        case HIDKey::P: return "P";
        case HIDKey::Q: return "Q";
        case HIDKey::R: return "R";
        case HIDKey::S: return "S";
        case HIDKey::T: return "T";
        case HIDKey::U: return "U";
        case HIDKey::V: return "V";
        case HIDKey::W: return "W";
        case HIDKey::X: return "X";
        case HIDKey::Y: return "Y";
        case HIDKey::Z: return "Z";
        case HIDKey::NUM_1: return "1";
        case HIDKey::NUM_2: return "2";
        case HIDKey::NUM_3: return "3";
        case HIDKey::NUM_4: return "4";
        case HIDKey::NUM_5: return "5";
        case HIDKey::NUM_6: return "6";
        case HIDKey::NUM_7: return "7";
        case HIDKey::NUM_8: return "8";
        case HIDKey::NUM_9: return "9";
        case HIDKey::NUM_0: return "0";
        case HIDKey::ENTER: return "Enter";
        case HIDKey::ESC: return "Esc";
        case HIDKey::BACKSPACE: return "Backspace";
        case HIDKey::TAB: return "Tab";
        case HIDKey::SPACE: return "Space";
        case HIDKey::MINUS: return "-";
        case HIDKey::EQUAL: return "=";
        case HIDKey::LEFT_BRACE: return "[";
        case HIDKey::RIGHT_BRACE: return "]";
        case HIDKey::BACKSLASH: return "\\";
        case HIDKey::NON_US_HASH: return "#";
        case HIDKey::SEMICOLON: return ";";
        case HIDKey::APOSTROPHE: return "'";
        case HIDKey::GRAVE: return "`";
        case HIDKey::COMMA: return ",";
        case HIDKey::DOT: return ".";
        case HIDKey::SLASH: return "/";
        case HIDKey::CAPS_LOCK: return "CapsLock";
        case HIDKey::F1: return "F1";
        case HIDKey::F2: return "F2";
        case HIDKey::F3: return "F3";
        case HIDKey::F4: return "F4";
        case HIDKey::F5: return "F5";
        case HIDKey::F6: return "F6";
        case HIDKey::F7: return "F7";
        case HIDKey::F8: return "F8";
        case HIDKey::F9: return "F9";
        case HIDKey::F10: return "F10";
        case HIDKey::F11: return "F11";
        case HIDKey::F12: return "F12";
        case HIDKey::PRINTSCREEN: return "PrintScreen";
        case HIDKey::SCROLL_LOCK: return "ScrollLock";
        case HIDKey::PAUSE: return "Pause";
        case HIDKey::INSERT: return "Insert";
        case HIDKey::HOME: return "Home";
        case HIDKey::PAGE_UP: return "PageUp";
        case HIDKey::DELETE: return "Delete";
        case HIDKey::END: return "End";
        case HIDKey::PAGE_DOWN: return "PageDown";
        case HIDKey::RIGHT: return "Right";
        case HIDKey::LEFT: return "Left";
        case HIDKey::DOWN: return "Down";
        case HIDKey::UP: return "Up";
        case HIDKey::NUM_LOCK: return "NumLock";
        case HIDKey::KP_SLASH: return "KP /";
        case HIDKey::KP_ASTERISK: return "KP *";
        case HIDKey::KP_MINUS: return "KP -";
        case HIDKey::KP_PLUS: return "KP +";
        case HIDKey::KP_ENTER: return "KP Enter";
        case HIDKey::KP_1: return "KP 1";
        case HIDKey::KP_2: return "KP 2";
        case HIDKey::KP_3: return "KP 3";
        case HIDKey::KP_4: return "KP 4";
        case HIDKey::KP_5: return "KP 5";
        case HIDKey::KP_6: return "KP 6";
        case HIDKey::KP_7: return "KP 7";
        case HIDKey::KP_8: return "KP 8";
        case HIDKey::KP_9: return "KP 9";
        case HIDKey::KP_0: return "KP 0";
        case HIDKey::KP_DOT: return "KP .";
        case HIDKey::NON_US_BACKSLASH: return "NonUSBackslash";
        case HIDKey::APPLICATION: return "Application";
        case HIDKey::POWER: return "Power";
        case HIDKey::KP_EQUAL: return "KP =";
        case HIDKey::F13: return "F13";
        case HIDKey::F14: return "F14";
        case HIDKey::F15: return "F15";
        case HIDKey::F16: return "F16";
        case HIDKey::F17: return "F17";
        case HIDKey::F18: return "F18";
        case HIDKey::F19: return "F19";
        case HIDKey::F20: return "F20";
        case HIDKey::F21: return "F21";
        case HIDKey::F22: return "F22";
        case HIDKey::F23: return "F23";
        case HIDKey::F24: return "F24";
        default: return "Unknown";
    }
}

#endif // HID_USAGE_H
