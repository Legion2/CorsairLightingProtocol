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
#pragma once

#include "Arduino.h"

#define toBigEndian(a) highByte(a), lowByte(a)

namespace CLP {
uint16_t fromBigEndian(const byte& byte1, const byte& byte2);

/**
 * Convert value from range 0-100 to 0-255
 *
 * @param value the value which should be converted
 */
inline uint8_t convert100To255(uint8_t value) { return value * 2.5546875f; }

/**
 * Convert value from range 0-255 to 0-100.
 *
 * @param value the value which should be converted
 */
inline uint8_t convert255To100(uint8_t value) { return value / 2.5546875f; }

/**
 * Check if a device id is the special null id (00 00 00 00).
 *
 * @param deviceId the device id to check
 */
bool isNullID(const uint8_t* deviceId);

/**
 * Check if a device id is the special reset id (FF FF FF FF).
 *
 * @param deviceId the device id to check
 */
bool isResetID(const uint8_t* deviceId);

/**
 * This will disable the RX and TX built in LEDs on Arduino Leonardo, Micro and Pro Micro.
 */
void disableBuildInLEDs();

/**
 * Print the given DeviceID to Serial
 *
 * @param deviceId the device id to print
 */
void printDeviceID(const uint8_t* deviceId);

/*
 * Measure and print the framerate at the given interval in milliseconds. The higher this value the more precise the
 * result will be. This function should be called after FastLED.show() to count the FPS.
 *
 * @param interval the measurement interval in milliseconds
 */
void printFps(const int interval);
}  // namespace CLP
