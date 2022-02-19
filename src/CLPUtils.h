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
#include "CorsairLightingFirmware.h"

#define toBigEndian(a) highByte(a), lowByte(a)

#if CLP_DEBUG

#define CLP_STRCAT(a, b) a##b
#define CLP_STRCAT3(a, b, c) a##b##c

#define CLP_XSTRCAT(a, b) CLP_STRCAT(a, b)
#define CLP_XSTRCAT3(a, b, c) CLP_STRCAT3(a, b, c)

#if defined(CLP_DEBUG_PORT)
namespace CLP {
extern int printf(const char* format, ...);
extern int printf(const __FlashStringHelper* format, ...);
}  // namespace CLP
#define clpPrintf CLP::printf
#else
#define clpPrintf printf
#endif

namespace CLP {
static inline void printVar(uint8_t const* buf, uint32_t bufsize) {
	for (uint32_t i = 0; i < bufsize; i++) clpPrintf("%02X ", buf[i]);
}
extern void printData(uint8_t const* buf, uint32_t bufsize, bool address_table);
}  // namespace CLP

/* clang-format off */
#define CLP_LOG(n, ...)               CLP_XSTRCAT(CLP_LOG, n)(__VA_ARGS__)
#define CLP_LOG_VAR(n, ...)           CLP_XSTRCAT3(CLP_LOG, n, _VAR)(__VA_ARGS__)
#define CLP_LOG_HEX(n, ...)           CLP_XSTRCAT3(CLP_LOG, n, _HEX)(__VA_ARGS__)
#define CLP_LOG_DAT(n, buf, size, at) CLP_XSTRCAT3(CLP_LOG, n, _DAT)(buf, size, at)
#define CLP_LOG_LOCATION()            clpPrintf("%s: %d:\r\n", __PRETTY_FUNCTION__, __LINE__)
#define CLP_LOG_FAILED()              clpPrintf("%s: %d: Failed\r\n", __PRETTY_FUNCTION__, __LINE__)
#define CLP_LOG_FUNC(func)            func

// Log Level 1: Error
#define CLP_LOG1                      clpPrintf
#define CLP_LOG1_VAR(_x)              CLP::printVar((uint8_t const*)(_x), sizeof(*(_x)))
#define CLP_LOG1_HEX(_x)              clpPrintf(#_x " = %lX\r\n", (unsigned long) (_x) )
#define CLP_LOG1_DAT(_x, _y, _z)      CLP::printData((uint8_t const*)(_x), _y, _z)

// Log Level 2: Warn
#if CLP_DEBUG >= 2
#define CLP_LOG2                      CLP_LOG1
#define CLP_LOG2_VAR                  CLP_LOG1_VAR
#define CLP_LOG2_HEX                  CLP_LOG1_HEX
#define CLP_LOG2_DAT                  CLP_LOG1_DAT
#endif

// Log Level 3: Info
#if CLP_DEBUG >= 3
#define CLP_LOG3                      CLP_LOG1
#define CLP_LOG3_VAR                  CLP_LOG1_VAR
#define CLP_LOG3_HEX                  CLP_LOG1_HEX
#define CLP_LOG3_DAT                  CLP_LOG1_DAT
#endif

// Log Level 4: Data
#if CLP_DEBUG >= 4
#define CLP_LOG4                      CLP_LOG1
#define CLP_LOG4_VAR                  CLP_LOG1_VAR
#define CLP_LOG4_HEX                  CLP_LOG1_HEX
#define CLP_LOG4_DAT                  CLP_LOG1_DAT
#endif
/* clang-format on */

#endif  // CLP_DEBUG

#ifndef CLP_LOG
#define CLP_LOG(n, ...)
#define CLP_LOG_VAR(n, ...)
#define CLP_LOG_HEX(n, ...)
#define CLP_LOG_DAT(n, buf, size, at)
#define CLP_LOG_FUNC(n)
#endif

#define CLP_LOG0(...)
#define CLP_LOG0_VAR(...)
#define CLP_LOG0_HEX(...)
#define CLP_LOG0_DAT(...)

#ifndef CLP_LOG1
#define CLP_LOG1(...)
#define CLP_LOG1_VAR(...)
#define CLP_LOG1_HEX(...)
#define CLP_LOG1_DAT(...)
#endif

#ifndef CLP_LOG2
#define CLP_LOG2(...)
#define CLP_LOG2_VAR(...)
#define CLP_LOG2_HEX(...)
#define CLP_LOG2_DAT(...)
#endif

#ifndef CLP_LOG3
#define CLP_LOG3(...)
#define CLP_LOG3_VAR(...)
#define CLP_LOG3_HEX(...)
#define CLP_LOG3_DAT(...)
#endif

#ifndef CLP_LOG4
#define CLP_LOG4(...)
#define CLP_LOG4_VAR(...)
#define CLP_LOG4_HEX(...)
#define CLP_LOG4_DAT(...)
#endif

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
bool isNullID(const DeviceID& deviceId);

/**
 * Check if a device id is the special reset id (FF FF FF FF).
 *
 * @param deviceId the device id to check
 */
bool isResetID(const DeviceID& deviceId);

/**
 * This will disable the RX and TX built in LEDs on Arduino Leonardo, Micro and Pro Micro.
 */
void disableBuildInLEDs();

/**
 * Print the given DeviceID to Serial
 *
 * @param deviceId the device id to print
 */
void printDeviceID(const DeviceID& deviceId);

/*
 * Measure and print the framerate at the given interval in milliseconds. The higher this value the more precise the
 * result will be. This function should be called after FastLED.show() to count the FPS.
 *
 * @param interval the measurement interval in milliseconds
 */
void printFps(const int interval);
}  // namespace CLP
