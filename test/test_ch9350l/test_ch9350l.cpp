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

void setup() {
    delay(200);
    UNITY_BEGIN();
    RUN_TEST(test_keyboard_frame_parsing);
    UNITY_END();
}

void loop() {}
