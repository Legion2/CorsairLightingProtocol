/*
 * Raspberry Pi Pico 2 -> Corsair Lighting Node PRO compatible ARGB controller
 *
 * USB device: Corsair Lighting Node PRO (VID 0x1B1C, PID 0x0C0B)
 * Channel 1: GP2
 * Channel 2: GP3
 * LED protocol: WS2812B-compatible, GRB order
 */

#include <CorsairLightingProtocol.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <stddef.h>
#include <string.h>

namespace {

constexpr uint8_t kChannel1DataPin = 2;
constexpr uint8_t kChannel2DataPin = 3;
// OpenRGB's Lighting Node PRO driver permits up to 204 LEDs in each zone.
constexpr uint8_t kLedsPerChannel = 204;
constexpr size_t kChannelBytes = kLedsPerChannel * sizeof(CRGB);
constexpr uint32_t kPersistentFrameMagic = 0x434C5032;  // "CLP2"
constexpr uint16_t kPersistentFrameVersion = 1;
// A static color must remain unchanged for this long before it is committed.
// Animated effects therefore do not continuously wear the flash sector.
constexpr unsigned long kPersistentFrameDelayMs = 10000;

static_assert(sizeof(CRGB) == 3, "Persistent frame format requires three-byte CRGB pixels");

struct PersistentFrame {
	uint32_t magic;
	uint16_t version;
	uint16_t ledsPerChannel;
	uint8_t channel1[kChannelBytes];
	uint8_t channel2[kChannelBytes];
	uint32_t checksum;
};

CRGB channel1[kLedsPerChannel];
CRGB channel2[kLedsPerChannel];

FastLEDController ledController(nullptr);
CorsairLightingFirmwareStorageStatic* firmwareStorage = nullptr;
CorsairLightingFirmware* firmware = nullptr;
CorsairLightingProtocolController* protocolController = nullptr;
CorsairLightingProtocolTinyUSBHID* usbHID = nullptr;
char usbSerialNumber[13] = {};

PersistentFrame savedFrame;
PersistentFrame candidateFrame;
bool savedFrameValid = false;
bool candidateFrameValid = false;
volatile bool frameUpdateObserved = false;
unsigned long candidateFrameSince = 0;

uint32_t calculateFrameChecksum(const PersistentFrame& frame) {
	// FNV-1a over every persisted field except the checksum itself.
	const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&frame);
	uint32_t hash = 2166136261u;
	for (size_t i = 0; i < offsetof(PersistentFrame, checksum); ++i) {
		hash ^= bytes[i];
		hash *= 16777619u;
	}
	return hash;
}

bool isValidPersistentFrame(const PersistentFrame& frame) {
	return frame.magic == kPersistentFrameMagic && frame.version == kPersistentFrameVersion &&
		   frame.ledsPerChannel == kLedsPerChannel && frame.checksum == calculateFrameChecksum(frame);
}

void captureCurrentFrame(PersistentFrame& frame) {
	frame.magic = kPersistentFrameMagic;
	frame.version = kPersistentFrameVersion;
	frame.ledsPerChannel = kLedsPerChannel;
	::memcpy(frame.channel1, channel1, kChannelBytes);
	::memcpy(frame.channel2, channel2, kChannelBytes);
	frame.checksum = calculateFrameChecksum(frame);
}

bool framePixelsEqual(const PersistentFrame& lhs, const PersistentFrame& rhs) {
	return ::memcmp(lhs.channel1, rhs.channel1, kChannelBytes) == 0 &&
		   ::memcmp(lhs.channel2, rhs.channel2, kChannelBytes) == 0;
}

bool currentPixelsEqual(const PersistentFrame& frame) {
	return ::memcmp(frame.channel1, channel1, kChannelBytes) == 0 &&
		   ::memcmp(frame.channel2, channel2, kChannelBytes) == 0;
}

void markFrameUpdated() {
	frameUpdateObserved = true;
}

uint8_t hexNibble(char value) {
	if (value >= '0' && value <= '9') {
		return value - '0';
	}
	if (value >= 'A' && value <= 'F') {
		return value - 'A' + 10;
	}
	if (value >= 'a' && value <= 'f') {
		return value - 'a' + 10;
	}
	return 0;
}

void beginUsbProtocol() {
	// Arduino-Pico exposes the RP2350's stable 64-bit board ID as 16 hex
	// characters. The first 12 preserve a Corsair-style USB serial, while the
	// final 8 become the four-byte CLP device ID used by OpenRGB matching.
	const char* chipID = rp2040.getChipID();
	const size_t chipIDLength = ::strlen(chipID);
	const size_t usbSerialLength =
		chipIDLength < sizeof(usbSerialNumber) - 1 ? chipIDLength : sizeof(usbSerialNumber) - 1;
	::memcpy(usbSerialNumber, chipID, usbSerialLength);
	usbSerialNumber[usbSerialLength] = '\0';

	DeviceID deviceID = {};
	const size_t deviceIDOffset = chipIDLength > 8 ? chipIDLength - 8 : 0;
	for (size_t index = 0; index < sizeof(deviceID.data); ++index) {
		const size_t characterOffset = deviceIDOffset + index * 2;
		if (characterOffset + 1 < chipIDLength) {
			deviceID.data[index] = (hexNibble(chipID[characterOffset]) << 4) | hexNibble(chipID[characterOffset + 1]);
		}
	}

	firmwareStorage = new CorsairLightingFirmwareStorageStatic(deviceID);
	firmware = new CorsairLightingFirmware(CORSAIR_LIGHTING_NODE_PRO, firmwareStorage);
	protocolController = new CorsairLightingProtocolController(&ledController, firmware);
	usbHID = new CorsairLightingProtocolTinyUSBHID(protocolController, usbSerialNumber);
	usbHID->setup();
}

void beginPersistentFrameStorage() {
	EEPROM.begin(sizeof(PersistentFrame));
	EEPROM.get(0, savedFrame);
	savedFrameValid = isValidPersistentFrame(savedFrame);

	if (savedFrameValid) {
		::memcpy(channel1, savedFrame.channel1, kChannelBytes);
		::memcpy(channel2, savedFrame.channel2, kChannelBytes);
	} else {
		::memset(channel1, 0, kChannelBytes);
		::memset(channel2, 0, kChannelBytes);
	}

	captureCurrentFrame(candidateFrame);
	candidateFrameValid = true;
	candidateFrameSince = millis();
}

void updatePersistentFrameStorage() {
	const unsigned long now = millis();

	if (frameUpdateObserved) {
		frameUpdateObserved = false;
		if (!candidateFrameValid || !currentPixelsEqual(candidateFrame)) {
			captureCurrentFrame(candidateFrame);
			candidateFrameValid = true;
			candidateFrameSince = now;
		}
	}

	if (!candidateFrameValid || now - candidateFrameSince < kPersistentFrameDelayMs) {
		return;
	}

	if (savedFrameValid && framePixelsEqual(candidateFrame, savedFrame)) {
		return;
	}

	EEPROM.put(0, candidateFrame);
	if (EEPROM.commit()) {
		savedFrame = candidateFrame;
		savedFrameValid = true;
	}
}

}  // namespace

void setup() {
	FastLED.addLeds<WS2812B, kChannel1DataPin, GRB>(channel1, kLedsPerChannel);
	FastLED.addLeds<WS2812B, kChannel2DataPin, GRB>(channel2, kLedsPerChannel);
	ledController.addLEDs(0, channel1, kLedsPerChannel);
	ledController.addLEDs(1, channel2, kLedsPerChannel);
	ledController.onUpdateHook(0, markFrameUpdated);
	ledController.onUpdateHook(1, markFrameUpdated);

	beginPersistentFrameStorage();
	FastLED.show();

	beginUsbProtocol();
}

void loop() {
	usbHID->update();

	if (ledController.updateLEDs()) {
		FastLED.show();
	}

	updatePersistentFrameStorage();
}
