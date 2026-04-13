#include "PS2Transcoder.h"

PS2Transcoder::PS2Transcoder() {
    _pressedCount = 0;
    _prevModifiers = 0;
}

static inline void appendBytes(PS2Data &dst, const uint8_t *src, uint8_t len) {
    if (dst.len + len > sizeof(dst.data)) return;
    for (uint8_t i = 0; i < len; ++i) dst.data[dst.len++] = src[i];
}

PS2Data PS2Transcoder::onKeyDown(HIDKey key, uint8_t modifiers) {
    PS2Data out = {0, {0}};

    // modifiers first
    PS2Data mod = _updateModifiers(modifiers);
    appendBytes(out, mod.data, mod.len);

    if (key != HIDKey::NONE) {
        PS2Data make = _getMakeCodes(key);
        appendBytes(out, make.data, make.len);
        // add to pressed list if not present
        bool found = false;
        for (uint8_t i = 0; i < _pressedCount; ++i) if (_pressedKeys[i] == key) { found = true; break; }
        if (!found && _pressedCount < 6) _pressedKeys[_pressedCount++] = key;
    }

    return out;
}

PS2Data PS2Transcoder::onKeyUp(HIDKey key, uint8_t modifiers) {
    PS2Data out = {0, {0}};

    if (key != HIDKey::NONE) {
        PS2Data br = _getBreakCodes(key);
        appendBytes(out, br.data, br.len);
        // remove from pressed list
        for (uint8_t i = 0; i < _pressedCount; ++i) {
            if (_pressedKeys[i] == key) {
                // shift down
                for (uint8_t j = i; j + 1 < _pressedCount; ++j) _pressedKeys[j] = _pressedKeys[j+1];
                _pressedCount--;
                break;
            }
        }
    }

    PS2Data mod = _updateModifiers(modifiers);
    appendBytes(out, mod.data, mod.len);

    return out;
}

PS2Data PS2Transcoder::_getMakeCodes(HIDKey key) {
    return getPS2MakeCode(key);
}

PS2Data PS2Transcoder::_getBreakCodes(HIDKey key) {
    PS2Data make = _getMakeCodes(key);
    PS2Data out = {0, {0}};
    if (make.len == 0) return out;
    if (make.len == 2 && make.data[0] == 0xE0) {
        uint8_t seq[3] = {0xE0, 0xF0, make.data[1]};
        appendBytes(out, seq, 3);
    } else if (make.len == 1) {
        uint8_t seq[2] = {0xF0, make.data[0]};
        appendBytes(out, seq, 2);
    }
    return out;
}

PS2Data PS2Transcoder::_updateModifiers(uint8_t newModifiers) {
    PS2Data out = {0, {0}};
    uint8_t diff = newModifiers ^ _prevModifiers;
    if (diff == 0) return out;

    // LCTRL: 0x01
    if (diff & static_cast<uint8_t>(PS2Modifier::LCTRL)) {
        if (newModifiers & static_cast<uint8_t>(PS2Modifier::LCTRL)) { uint8_t a[]={0x14}; appendBytes(out,a,1); }
        else { uint8_t a[]={0xF0,0x14}; appendBytes(out,a,2); }
    }
    // LSHIFT: 0x02
    if (diff & static_cast<uint8_t>(PS2Modifier::LSHIFT)) {
        if (newModifiers & static_cast<uint8_t>(PS2Modifier::LSHIFT)) { uint8_t a[]={0x12}; appendBytes(out,a,1); }
        else { uint8_t a[]={0xF0,0x12}; appendBytes(out,a,2); }
    }
    // LALT: 0x04
    if (diff & static_cast<uint8_t>(PS2Modifier::LALT)) {
        if (newModifiers & static_cast<uint8_t>(PS2Modifier::LALT)) { uint8_t a[]={0x11}; appendBytes(out,a,1); }
        else { uint8_t a[]={0xF0,0x11}; appendBytes(out,a,2); }
    }
    // LGUI: 0x08
    if (diff & static_cast<uint8_t>(PS2Modifier::LGUI)) {
        if (newModifiers & static_cast<uint8_t>(PS2Modifier::LGUI)) { uint8_t a[]={0xE0,0x1F}; appendBytes(out,a,2); }
        else { uint8_t a[]={0xE0,0xF0,0x1F}; appendBytes(out,a,3); }
    }
    // RCTRL: 0x10
    if (diff & static_cast<uint8_t>(PS2Modifier::RCTRL)) {
        if (newModifiers & static_cast<uint8_t>(PS2Modifier::RCTRL)) { uint8_t a[]={0xE0,0x14}; appendBytes(out,a,2); }
        else { uint8_t a[]={0xE0,0xF0,0x14}; appendBytes(out,a,3); }
    }
    // RSHIFT: 0x20
    if (diff & static_cast<uint8_t>(PS2Modifier::RSHIFT)) {
        if (newModifiers & static_cast<uint8_t>(PS2Modifier::RSHIFT)) { uint8_t a[]={0x59}; appendBytes(out,a,1); }
        else { uint8_t a[]={0xF0,0x59}; appendBytes(out,a,2); }
    }
    // RALT: 0x40
    if (diff & static_cast<uint8_t>(PS2Modifier::RALT)) {
        if (newModifiers & static_cast<uint8_t>(PS2Modifier::RALT)) { uint8_t a[]={0xE0,0x11}; appendBytes(out,a,2); }
        else { uint8_t a[]={0xE0,0xF0,0x11}; appendBytes(out,a,3); }
    }
    // RGUI: 0x80
    if (diff & static_cast<uint8_t>(PS2Modifier::RGUI)) {
        if (newModifiers & static_cast<uint8_t>(PS2Modifier::RGUI)) { uint8_t a[]={0xE0,0x27}; appendBytes(out,a,2); }
        else { uint8_t a[]={0xE0,0xF0,0x27}; appendBytes(out,a,3); }
    }

    _prevModifiers = newModifiers;
    return out;
}

// Mapping helper: returns 0-length PS2Data for unknown keys.
PS2Data getPS2MakeCode(HIDKey key) {
    PS2Data out = {0, {0}};
    switch (key) {
        case HIDKey::A: out.len = 1; out.data[0] = 0x1C; break;
        case HIDKey::B: out.len = 1; out.data[0] = 0x32; break;
        case HIDKey::C: out.len = 1; out.data[0] = 0x21; break;
        case HIDKey::D: out.len = 1; out.data[0] = 0x23; break;
        case HIDKey::E: out.len = 1; out.data[0] = 0x24; break;
        case HIDKey::F: out.len = 1; out.data[0] = 0x2B; break;
        case HIDKey::G: out.len = 1; out.data[0] = 0x34; break;
        case HIDKey::H: out.len = 1; out.data[0] = 0x33; break;
        case HIDKey::I: out.len = 1; out.data[0] = 0x43; break;
        case HIDKey::J: out.len = 1; out.data[0] = 0x3B; break;
        case HIDKey::K: out.len = 1; out.data[0] = 0x42; break;
        case HIDKey::L: out.len = 1; out.data[0] = 0x4B; break;
        case HIDKey::M: out.len = 1; out.data[0] = 0x3A; break;
        case HIDKey::N: out.len = 1; out.data[0] = 0x31; break;
        case HIDKey::O: out.len = 1; out.data[0] = 0x44; break;
        case HIDKey::P: out.len = 1; out.data[0] = 0x4D; break;
        case HIDKey::Q: out.len = 1; out.data[0] = 0x15; break;
        case HIDKey::R: out.len = 1; out.data[0] = 0x2D; break;
        case HIDKey::S: out.len = 1; out.data[0] = 0x1B; break;
        case HIDKey::T: out.len = 1; out.data[0] = 0x2C; break;
        case HIDKey::U: out.len = 1; out.data[0] = 0x3C; break;
        case HIDKey::V: out.len = 1; out.data[0] = 0x2A; break;
        case HIDKey::W: out.len = 1; out.data[0] = 0x1D; break;
        case HIDKey::X: out.len = 1; out.data[0] = 0x22; break;
        case HIDKey::Y: out.len = 1; out.data[0] = 0x35; break;
        case HIDKey::Z: out.len = 1; out.data[0] = 0x1A; break;
        case HIDKey::NUM_1: out.len = 1; out.data[0] = 0x16; break;
        case HIDKey::NUM_2: out.len = 1; out.data[0] = 0x1E; break;
        case HIDKey::NUM_3: out.len = 1; out.data[0] = 0x26; break;
        case HIDKey::NUM_4: out.len = 1; out.data[0] = 0x25; break;
        case HIDKey::NUM_5: out.len = 1; out.data[0] = 0x2E; break;
        case HIDKey::NUM_6: out.len = 1; out.data[0] = 0x36; break;
        case HIDKey::NUM_7: out.len = 1; out.data[0] = 0x3D; break;
        case HIDKey::NUM_8: out.len = 1; out.data[0] = 0x3E; break;
        case HIDKey::NUM_9: out.len = 1; out.data[0] = 0x46; break;
        case HIDKey::NUM_0: out.len = 1; out.data[0] = 0x45; break;
        case HIDKey::ENTER: out.len = 1; out.data[0] = 0x5A; break;
        case HIDKey::ESC: out.len = 1; out.data[0] = 0x76; break;
        case HIDKey::BACKSPACE: out.len = 1; out.data[0] = 0x66; break;
        case HIDKey::TAB: out.len = 1; out.data[0] = 0x0D; break;
        case HIDKey::SPACE: out.len = 1; out.data[0] = 0x29; break;
        case HIDKey::MINUS: out.len = 1; out.data[0] = 0x4E; break;
        case HIDKey::EQUAL: out.len = 1; out.data[0] = 0x55; break;
        case HIDKey::LEFT_BRACE: out.len = 1; out.data[0] = 0x54; break;
        case HIDKey::RIGHT_BRACE: out.len = 1; out.data[0] = 0x5B; break;
        case HIDKey::BACKSLASH: out.len = 1; out.data[0] = 0x5D; break;
        case HIDKey::SEMICOLON: out.len = 1; out.data[0] = 0x4C; break;
        case HIDKey::APOSTROPHE: out.len = 1; out.data[0] = 0x52; break;
        case HIDKey::GRAVE: out.len = 1; out.data[0] = 0x0E; break;
        case HIDKey::COMMA: out.len = 1; out.data[0] = 0x41; break;
        case HIDKey::DOT: out.len = 1; out.data[0] = 0x49; break;
        case HIDKey::SLASH: out.len = 1; out.data[0] = 0x4A; break;
        case HIDKey::CAPS_LOCK: out.len = 1; out.data[0] = 0x58; break;
        case HIDKey::F1: out.len = 1; out.data[0] = 0x05; break;
        case HIDKey::F2: out.len = 1; out.data[0] = 0x06; break;
        case HIDKey::F3: out.len = 1; out.data[0] = 0x04; break;
        case HIDKey::F4: out.len = 1; out.data[0] = 0x0C; break;
        case HIDKey::F5: out.len = 1; out.data[0] = 0x03; break;
        case HIDKey::F6: out.len = 1; out.data[0] = 0x0B; break;
        case HIDKey::F7: out.len = 1; out.data[0] = 0x83; break;
        case HIDKey::F8: out.len = 1; out.data[0] = 0x0A; break;
        case HIDKey::F9: out.len = 1; out.data[0] = 0x01; break;
        case HIDKey::F10: out.len = 1; out.data[0] = 0x09; break;
        case HIDKey::F11: out.len = 1; out.data[0] = 0x78; break;
        case HIDKey::F12: out.len = 1; out.data[0] = 0x07; break;
        case HIDKey::INSERT: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x70; break;
        case HIDKey::HOME: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x6C; break;
        case HIDKey::PAGE_UP: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x7D; break;
        case HIDKey::DELETE: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x71; break;
        case HIDKey::END: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x69; break;
        case HIDKey::PAGE_DOWN: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x7A; break;
        case HIDKey::RIGHT: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x74; break;
        case HIDKey::LEFT: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x6B; break;
        case HIDKey::DOWN: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x72; break;
        case HIDKey::UP: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x75; break;
        case HIDKey::NUM_LOCK: out.len = 1; out.data[0] = 0x77; break;
        case HIDKey::KP_SLASH: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x4A; break;
        case HIDKey::KP_ASTERISK: out.len = 1; out.data[0] = 0x7C; break;
        case HIDKey::KP_MINUS: out.len = 1; out.data[0] = 0x7B; break;
        case HIDKey::KP_PLUS: out.len = 1; out.data[0] = 0x79; break;
        case HIDKey::KP_ENTER: out.len = 2; out.data[0] = 0xE0; out.data[1] = 0x5A; break;
        case HIDKey::KP_1: out.len = 1; out.data[0] = 0x69; break;
        case HIDKey::KP_2: out.len = 1; out.data[0] = 0x72; break;
        case HIDKey::KP_3: out.len = 1; out.data[0] = 0x7A; break;
        case HIDKey::KP_4: out.len = 1; out.data[0] = 0x6B; break;
        case HIDKey::KP_5: out.len = 1; out.data[0] = 0x73; break;
        case HIDKey::KP_6: out.len = 1; out.data[0] = 0x74; break;
        case HIDKey::KP_7: out.len = 1; out.data[0] = 0x6C; break;
        case HIDKey::KP_8: out.len = 1; out.data[0] = 0x75; break;
        case HIDKey::KP_9: out.len = 1; out.data[0] = 0x7D; break;
        case HIDKey::KP_0: out.len = 1; out.data[0] = 0x70; break;
        case HIDKey::KP_DOT: out.len = 1; out.data[0] = 0x71; break;
        default: break;
    }
    return out;
}

String bytesToHexString(const uint8_t *bytes, size_t len) {
    String s;
    for (size_t i = 0; i < len; ++i) {
        if (bytes[i] < 16) s += '0';
        s += String(bytes[i], HEX);
        if (i + 1 < len) s += ' ';
    }
    return s;
}
