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
#ifndef _CORSAIRLIGHTINGFIRMWARE_h
#define _CORSAIRLIGHTINGFIRMWARE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CorsairLightingProtocol.h"
// VID: 1b1c
// PID: 0c0b
// Revision: 0001
// Manufacturer: Corsair
// Product Name: Lighting Node PRO
// Serial Number: 1EA000060904BAAEFB66DF55421900F5

#define PROTOCOL_STATUS_OK 0x00
#define PROTOCOL_STATUS_ERROR 0xFF

const uint8_t firmware_version[] PROGMEM = { 0x00, 0x02, 0x36 };
const uint8_t bootloader_version[] PROGMEM = { 0x00, 0x02 };
const uint8_t status[] PROGMEM = { PROTOCOL_STATUS_OK };

class CorsairLightingFirmware_ {
public:
	void handleFirmwareCommand(const Command& command);
protected:
	uint8_t DeviceId[4] = { 0x4E, 0x91, 0x67, 0xB9 };
};

CorsairLightingFirmware_& CorsairLightingFirmware();

#endif

