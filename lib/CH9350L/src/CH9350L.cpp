#include "CH9350L.h"

CH9350L::CH9350L(Stream &stream)
{
    _stream = &stream;
}

void CH9350L::begin(unsigned long baud)
{
    if (_beginFn) {
        _beginFn(_beginContext, baud);
    }
}

void CH9350L::update()
{
    if (!_stream) return;
    while (_stream->available()) {
        uint8_t b = static_cast<uint8_t>(_stream->read());
        _parseByte(b);
    }
}

void CH9350L::onKeyDown(KeyCallback cb)
{
    _keyDownCb = cb;
}

void CH9350L::onKeyUp(KeyCallback cb)
{
    _keyUpCb = cb;
}

void CH9350L::onModifiersChanged(ModCallback cb)
{
    _modCb = cb;
}

bool CH9350L::sendCommand(const uint8_t *data, size_t len)
{
    if (!_stream) return false;
    size_t written = _stream->write(data, len);
    return written == len;
}

// CH9350L frame parser based on datasheet
// Frames start with 0x57 0xAB then a type byte. For common payloads:
//  - 0x01 : 8-byte keyboard report (USB standard: [mods][reserved][k1..k6])
//  - 0x02 : 4-byte relative mouse report (button, dx, dy, wheel)
//  - 0x04 : 7-byte absolute mouse report
// Other types exist (e.g., 0x83/0x88 with length field); unknown frames are skipped.
void CH9350L::_parseByte(uint8_t b)
{
    switch (_state) {
        case PS_WAIT_57:
            if (b == 0x57) _state = PS_WAIT_AB;
            break;
        case PS_WAIT_AB:
            if (b == 0xAB) {
                _state = PS_READ_TYPE;
            } else {
                _state = (b == 0x57) ? PS_WAIT_AB : PS_WAIT_57;
            }
            break;
        case PS_READ_TYPE:
            _frameType = b;
            _payloadIdx = 0;
            // determine expected payload length for a few common frame types
            if (_frameType == 0x01) {
                _expected = 8; // keyboard
                _state = PS_READ_PAYLOAD;
            } else if (_frameType == 0x02) {
                _expected = 4; // relative mouse
                _state = PS_READ_PAYLOAD;
            } else if (_frameType == 0x04) {
                _expected = 7; // absolute mouse
                _state = PS_READ_PAYLOAD;
            } else if (_frameType == 0x83 || _frameType == 0x88 || _frameType == 0x81) {
                // These frames include a length field after the type. We'll read that next
                _expected = 0xFF; // indicator to read length as first payload byte
                _state = PS_READ_PAYLOAD;
            } else {
                // Unknown frame type: skip until next header
                _state = PS_WAIT_57;
            }
            break;
        case PS_READ_PAYLOAD:
            if (_expected == 0xFF) {
                // first payload byte is length (for frames that include length)
                _expected = b; // total payload length to skip/read
                _payloadIdx = 0;
                if (_expected == 0) {
                    _state = PS_WAIT_57;
                }
                // consume/skip the payload by staying in this state until done
                return;
            }

            if (_payloadIdx < sizeof(_payload)) {
                _payload[_payloadIdx++] = b;
            }

            if (_payloadIdx >= _expected) {
                // Full payload received — handle known frames
                if (_frameType == 0x01 && _expected == 8) {
                    // keyboard report: [mods][reserved][k1..k6]
                    uint8_t mods = _payload[0];
                    uint8_t currKeys[6];
                    for (uint8_t i = 0; i < 6; ++i) currKeys[i] = _payload[2 + i];

                    // detect key downs: present in curr but not in prev
                    for (uint8_t i = 0; i < 6; ++i) {
                        uint8_t k = currKeys[i];
                        if (k == 0) continue;
                        bool found = false;
                        for (uint8_t j = 0; j < 6; ++j) if (_prevKeys[j] == k) { found = true; break; }
                        if (!found && _keyDownCb) _keyDownCb(k, mods);
                    }

                    // detect key ups: present in prev but not in curr
                    for (uint8_t i = 0; i < 6; ++i) {
                        uint8_t k = _prevKeys[i];
                        if (k == 0) continue;
                        bool found = false;
                        for (uint8_t j = 0; j < 6; ++j) if (currKeys[j] == k) { found = true; break; }
                        if (!found && _keyUpCb) _keyUpCb(k, mods);
                    }

                    // modifiers changed?
                    if (mods != _prevMods && _modCb) {
                        _modCb(mods);
                    }

                    // update previous state
                    memcpy(_prevKeys, currKeys, 6);
                    _prevMods = mods;
                }

                // Reset to look for next frame
                _state = PS_WAIT_57;
                _payloadIdx = 0;
                _frameType = 0;
                _expected = 0;
            }
            break;
        default:
            _state = PS_WAIT_57;
            break;
    }
}
