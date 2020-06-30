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
#include "CorsairLightingProtocolConstants.h"
#include "CorsairLightingProtocolResponse.h"

#ifndef EEPROM_ADDRESS_DEVICE_ID
#define EEPROM_ADDRESS_DEVICE_ID 0
#endif

#define PROTOCOL_STATUS_OK 0x00
#define PROTOCOL_STATUS_ERROR 0xFF

#define FIRMWARE_VERSION_SIZE 3

class CorsairLightingFirmware {
public:
	CorsairLightingFirmware(const uint8_t* firmwareVersion);
	void handleFirmwareCommand(const Command& command, const CorsairLightingProtocolResponse* response);
	void getDeviceID(uint8_t* deviceID) const;
	void setDeviceID(const uint8_t* deviceID);
	uint8_t getStatus();
	void setStatus(uint8_t status);

protected:
	const uint8_t* firmwareVersion;
	uint8_t deviceId[4];

private:
	uint8_t status = PROTOCOL_STATUS_OK;
};

CorsairLightingFirmware corsairLightingNodePROFirmware();

CorsairLightingFirmware corsairLightingNodeCOREFirmware();

CorsairLightingFirmware corsairLS100Firmware();

CorsairLightingFirmware corsairCommanderPROFirmware();

CorsairLightingFirmware corsairLT100Firmware();
