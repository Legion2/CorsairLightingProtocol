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
#include "CorsairLightingFirmware.h"

#include <EEPROM.h>

const uint8_t bootloader_version[] PROGMEM = {0x00, 0x02};

const uint8_t corsairLightingNodePROFirmwareVersion[FIRMWARE_VERSION_SIZE] PROGMEM = {0x00, 0x0A, 0x04};

const uint8_t corsairCommanderPROFirmwareVersion[FIRMWARE_VERSION_SIZE] PROGMEM = {0x00, 0x09, 0xD4};

const uint8_t corsairLT100FirmwareVersion[FIRMWARE_VERSION_SIZE] PROGMEM = {0x01, 0x01, 0x38};

CorsairLightingFirmware::CorsairLightingFirmware(const uint8_t* firmwareVersion) : firmwareVersion(firmwareVersion) {
	EEPROM.get(EEPROM_ADDRESS_DEVICE_ID, deviceId);
}

void CorsairLightingFirmware::handleFirmwareCommand(const Command& command,
													const CorsairLightingProtocolResponse* response) {
	switch (command.command) {
		case READ_STATUS: {
			uint8_t statusData[] = {status};
			response->send(statusData, sizeof(statusData));
			break;
		}
		case READ_FIRMWARE_VERSION: {
			response->send_P(firmwareVersion, FIRMWARE_VERSION_SIZE);
			break;
		}
		case READ_DEVICE_ID: {
			response->send(deviceId, sizeof(deviceId));
			break;
		}
		case WRITE_DEVICE_ID: {
			setDeviceID(command.data);
			response->send(deviceId, sizeof(deviceId));
			break;
		}
		case READ_BOOTLOADER_VERSION: {
			response->send_P(bootloader_version, sizeof(bootloader_version));
			break;
		}
		default: {
			response->sendError();
		}
	}
}

void CorsairLightingFirmware::getDeviceID(uint8_t* deviceID) const { memcpy(deviceID, deviceId, sizeof(deviceId)); }

void CorsairLightingFirmware::setDeviceID(const uint8_t* deviceID) {
	memcpy(deviceId, deviceID, sizeof(deviceId));
	EEPROM.put(EEPROM_ADDRESS_DEVICE_ID, deviceId);
}

uint8_t CorsairLightingFirmware::getStatus() { return status; }

void CorsairLightingFirmware::setStatus(uint8_t a_status) { status = a_status; }

CorsairLightingFirmware corsairLightingNodePROFirmware() {
	return CorsairLightingFirmware(corsairLightingNodePROFirmwareVersion);
}

CorsairLightingFirmware corsairLightingNodeCOREFirmware() {
	return CorsairLightingFirmware(corsairLightingNodePROFirmwareVersion);
}

CorsairLightingFirmware corsairLS100Firmware() {
	return CorsairLightingFirmware(corsairLightingNodePROFirmwareVersion);
}

CorsairLightingFirmware corsairCommanderPROFirmware() {
	return CorsairLightingFirmware(corsairCommanderPROFirmwareVersion);
}

CorsairLightingFirmware corsairLT100Firmware() { return CorsairLightingFirmware(corsairLT100FirmwareVersion); }
