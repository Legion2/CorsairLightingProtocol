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

#include "Arduino.h"

#include "CorsairLightingProtocolResponse.h"
#include "CorsairLightingProtocolConstants.h"

#ifndef EEPROM_ADDRESS_DEVICE_ID
#define EEPROM_ADDRESS_DEVICE_ID 0
#endif

#define PROTOCOL_STATUS_OK 0x00
#define PROTOCOL_STATUS_ERROR 0xFF

#define FIRMWARE_VERSION_SIZE 3

const uint8_t bootloader_version[] PROGMEM = { 0x00, 0x02 };

class CorsairLightingFirmware {
public:
	CorsairLightingFirmware(const uint8_t* firmwareVersion);
	void handleFirmwareCommand(const Command& command, const CorsairLightingProtocolResponse* response);
protected:
	const uint8_t* firmwareVersion;
	uint8_t DeviceId[4];
};

#endif

