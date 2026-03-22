#include <Arduino.h>
#include <unity.h>
#include "CH9350L.h"

// Simple FakeStream to feed bytes into CH9350L::update()
class FakeStream : public Stream {
public:
    FakeStream() : _readPos(0), _writePos(0) {}
    void append(const uint8_t *data, size_t len) {
        for (size_t i=0;i<len && _writePos < sizeof(_buf); ++i) _buf[_writePos++] = data[i];
    }
    int available() override { return _writePos - _readPos; }
    int read() override { if (_readPos < _writePos) return _buf[_readPos++]; return -1; }
    int peek() override { if (_readPos < _writePos) return _buf[_readPos]; return -1; }
    void flush() override {}
    size_t write(uint8_t) override { return 0; }
private:
    uint8_t _buf[256];
    size_t _readPos, _writePos;
};

static bool keyDownCalled = false;
static bool keyUpCalled = false;
static bool modsCalled = false;

void kd(uint8_t k, uint8_t m) { keyDownCalled = true; }
void ku(uint8_t k, uint8_t m) { keyUpCalled = true; }
void mm(uint8_t m) { modsCalled = true; }

void test_keyboard_frame_parsing(void) {
    FakeStream fs;
    CH9350L ch(fs);
    ch.onKeyDown(kd);
    ch.onKeyUp(ku);
    ch.onModifiersChanged(mm);

    // Example keyboard press: header + type 0x01 + 8-byte report
    // mods=0x02 (LShift), reserved=0x00, key[0]=0x2C(space)
    uint8_t frame[] = {0x57,0xAB,0x01, 0x02,0x00, 0x2C,0x00,0x00,0x00,0x00,0x00};
    fs.append(frame, sizeof(frame));
    ch.update();
    TEST_ASSERT_TRUE(keyDownCalled);
    TEST_ASSERT_TRUE(modsCalled);
}

// --- LED/frame emission test ---
// Fake stream that captures written bytes for inspection
class FakeWriteStream : public Stream {
public:
    FakeWriteStream() : len(0) {}
    int available() override { return 0; }
    int read() override { return -1; }
    int peek() override { return -1; }
    void flush() override {}
    size_t write(uint8_t ch) override { if (len < sizeof(buf)) buf[len++] = ch; return 1; }
    size_t write(const uint8_t *data, size_t size) override { for (size_t i = 0; i < size && len < sizeof(buf); ++i) buf[len++] = data[i]; return size; }

    uint8_t buf[64];
    size_t len;
};

void test_set_keyboard_indicators_frame(void) {
    FakeWriteStream fs;
    CH9350L ch(fs);

    // request Num + Caps
    bool ok = ch.setKeyboardIndicators(CH9350L::LED_NUM | CH9350L::LED_CAPS);
    TEST_ASSERT_TRUE(ok);

    // Expect 11-byte frame: 0x57,0xAB,0x12, <4 pid/zeros>, 0xFF,0xFF, <led byte>, 0x20
    TEST_ASSERT_EQUAL_UINT8(11, fs.len);
    TEST_ASSERT_EQUAL_HEX8(0x57, fs.buf[0]);
    TEST_ASSERT_EQUAL_HEX8(0xAB, fs.buf[1]);
    TEST_ASSERT_EQUAL_HEX8(0x12, fs.buf[2]);
    // leds at position 9 (0-based)
    TEST_ASSERT_EQUAL_HEX8((uint8_t)(CH9350L::LED_NUM | CH9350L::LED_CAPS), fs.buf[9]);
    TEST_ASSERT_EQUAL_HEX8(0x20, fs.buf[10]);
}

void setup() {
    delay(200);
    UNITY_BEGIN();
    RUN_TEST(test_keyboard_frame_parsing);
    UNITY_END();
}

void loop() {}
