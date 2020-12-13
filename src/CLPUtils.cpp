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

bool CLP::isNullID(const uint8_t* deviceId) { return !(deviceId[0] | deviceId[1] | deviceId[2] | deviceId[3]); }

bool CLP::isResetID(const uint8_t* deviceId) {
	return deviceId[0] == 0xFF && deviceId[1] == 0xFF && deviceId[2] == 0xFF && deviceId[3] == 0xFF;
}

void CLP::printDeviceID(const uint8_t* deviceId) {
	char tmp[16];
	for (size_t i = 0; i < 4; i++) {
		sprintf(tmp, "%.2X", deviceId[i]);
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
