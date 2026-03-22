#ifndef CH9350L_H
#define CH9350L_H

#include <Arduino.h>

class CH9350L {
public:
    using KeyCallback = void(*)(uint8_t keycode, uint8_t modifiers);
    using ModCallback = void(*)(uint8_t modifiers);

    // Accept any Stream (HardwareSerial, SoftwareSerial, etc.)
    explicit CH9350L(Stream &stream);

    // Template constructor records a callable that will invoke `begin(baud)`
    template<typename T>
    explicit CH9350L(T &serial) : _stream(&serial) {
        _beginContext = static_cast<void*>(&serial);
        _beginFn = +[](void *ctx, unsigned long baud) {
            static_cast<T*>(ctx)->begin(baud);
        };
    }

    // If constructed with a type that supports begin(), this will call it.
    void begin(unsigned long baud = 115200);
    void update(); // call from main loop to process UART data

    void onKeyDown(KeyCallback cb);
    void onKeyUp(KeyCallback cb);
    void onModifiersChanged(ModCallback cb);

    bool sendCommand(const uint8_t *data, size_t len);

private:
    Stream *_stream = nullptr;
    void *_beginContext = nullptr;
    void (*_beginFn)(void*, unsigned long) = nullptr;

    KeyCallback _keyDownCb = nullptr;
    KeyCallback _keyUpCb = nullptr;
    ModCallback _modCb = nullptr;
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

#endif // CH9350L_H
#ifndef CH9350L_H
#define CH9350L_H

#include <Arduino.h>

class CH9350L {
public:
	using KeyCallback = void(*)(uint8_t keycode, uint8_t modifiers);

	// Accept any Stream (HardwareSerial, SoftwareSerial, etc.)
	explicit CH9350L(Stream &stream);

	// Template constructor records a callable that will invoke `begin(baud)`
	template<typename T>
	explicit CH9350L(T &serial) : _stream(&serial) {
		_beginContext = static_cast<void*>(&serial);
		_beginFn = +[](void *ctx, unsigned long baud) {
			static_cast<T*>(ctx)->begin(baud);
		};
	}

	// If constructed with a type that supports begin(), this will call it.
	void begin(unsigned long baud = 115200);
	void update(); // call from main loop to process UART data

	void onKeyDown(KeyCallback cb);
	void onKeyUp(KeyCallback cb);

	bool sendCommand(const uint8_t *data, size_t len);

private:
	Stream *_stream = nullptr;
	void *_beginContext = nullptr;
	void (*_beginFn)(void*, unsigned long) = nullptr;

	KeyCallback _keyDownCb = nullptr;
	KeyCallback _keyUpCb = nullptr;

	// Simple parse state for incoming bytes. Replace with real CH9350L protocol parsing.
	void _parseByte(uint8_t b);
};

#endif // CH9350L_H
