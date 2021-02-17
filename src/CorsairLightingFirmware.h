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

#define FIRMWARE_VERSION_SIZE 3

const uint8_t corsairLightingNodePROFirmwareVersion[FIRMWARE_VERSION_SIZE] PROGMEM = {0x00, 0x0A, 0x04};

#define corsairLightingNodeCOREFirmwareVersion corsairLightingNodePROFirmwareVersion

#define corsairLS100FirmwareVersion corsairLightingNodePROFirmwareVersion

const uint8_t corsairCommanderPROFirmwareVersion[FIRMWARE_VERSION_SIZE] PROGMEM = {0x00, 0x09, 0xD4};

const uint8_t corsairLT100FirmwareVersion[FIRMWARE_VERSION_SIZE] PROGMEM = {0x01, 0x01, 0x38};

const uint8_t* const firmwareVersions[] PROGMEM = {
	corsairLightingNodePROFirmwareVersion, corsairCommanderPROFirmwareVersion, corsairLightingNodeCOREFirmwareVersion,
	corsairLS100FirmwareVersion, corsairLT100FirmwareVersion};

struct DeviceID {
	uint8_t data[4];
};

/**
 * Interface to store the device id
 */
class CorsairLightingFirmwareStorage {
public:
	virtual void loadDeviceID(DeviceID& deviceID) const = 0;
	virtual void saveDeviceID(const DeviceID& deviceID) = 0;
};

class CorsairLightingFirmware {
public:
	CorsairLightingFirmware(corsair_product_enum_t product, CorsairLightingFirmwareStorage* storage);
	void handleFirmwareCommand(const Command& command, const CorsairLightingProtocolResponse* response);
	void getDeviceID(DeviceID& deviceID) const;
	void setDeviceID(const DeviceID& deviceID);
	uint8_t getStatus();
	void setStatus(uint8_t status);
	uint8_t getProduct();

protected:
	CorsairLightingFirmwareStorage* const storage;
	const corsair_product_enum_t product;
	DeviceID deviceId;

private:
	uint8_t status = PROTOCOL_STATUS_OK;
};
