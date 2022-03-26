/*
   Copyright 2019 Leon Kiefer

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "CLPUtils.h"

uint16_t CLP::fromBigEndian(const byte& byte1, const byte& byte2) {
	uint16_t t = byte1;
	t = t << 8;
	t |= byte2;
	return t;
}

void CLP::disableBuildInLEDs() {
#ifndef DEBUG
#ifdef LED_BUILTIN_RX
	pinMode(LED_BUILTIN_RX, INPUT);
#endif
#ifdef LED_BUILTIN_TX
	pinMode(LED_BUILTIN_TX, INPUT);
#endif
#endif
}

bool CLP::isNullID(const DeviceID& deviceId) {
	return !(deviceId.data[0] | deviceId.data[1] | deviceId.data[2] | deviceId.data[3]);
}

bool CLP::isResetID(const DeviceID& deviceId) {
	return deviceId.data[0] == 0xFF && deviceId.data[1] == 0xFF && deviceId.data[2] == 0xFF && deviceId.data[3] == 0xFF;
}

void CLP::printDeviceID(const DeviceID& deviceId) {
	char tmp[16];
	for (size_t i = 0; i < 4; i++) {
		sprintf(tmp, "%.2X", deviceId.data[i]);
		Serial.print(tmp);
		if (i < 3) Serial.print(F(" "));
	}
}

void CLP::printFps(const int interval) {
	// Create static variables so that the code and variables can
	// all be declared inside a function
	static unsigned long lastMillis;
	static unsigned long frameCount;

	unsigned long now = millis();
	frameCount++;
	if (now - lastMillis >= (unsigned int)interval) {
		double framesPerSecond = (frameCount * 1000.0) / interval;
		Serial.print(F("FPS: "));
		Serial.println(framesPerSecond, 1);
		frameCount = 0;
		lastMillis = now;
	}
}

#if CLP_DEBUG
#if defined(CLP_DEBUG_PORT)

int CLP::printf(const char* __restrict format, ...) {
	char buf[64];
	int len;
	va_list ap;
	va_start(ap, format);
	len = vsnprintf(buf, sizeof(buf), format, ap);
	CLP_DEBUG_PORT.write(buf);
	va_end(ap);
	return len;
}

int CLP::printf(const __FlashStringHelper* __restrict format, ...) {
	char buf[64];
	char fmt[64];
	int len;
	va_list ap;
	va_start(ap, format);
	strcpy_P(fmt, (const char*)format);
	len = vsnprintf(buf, sizeof(buf), fmt, ap);
	CLP_DEBUG_PORT.write(buf);
	va_end(ap);
	return len;
}

#endif  // defined(CLP_DEBUG_PORT)

void CLP::printData(uint8_t const* buf, uint32_t bufsize, bool address_table) {
	if (address_table) {
		clpPrintf("\t>>>> ");
		for (uint32_t i = 0; i < 16; i++) clpPrintf("0x%X ", i);
		clpPrintf("\r\n");
	}
	for (uint32_t i = 0; i < bufsize; i++) {
		if (address_table && (i % 16 == 0)) clpPrintf("\t0x%02X ", (i / 16) << 4);
		clpPrintf(" %02X ", buf[i]);
		if ((i + 1) % 16 == 0 || (i + 1) == bufsize) clpPrintf("\r\n");
	}
}

#endif  // CLP_DEBUG
