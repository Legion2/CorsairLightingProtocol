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

const uint8_t bootloader_version[] PROGMEM = {0x00, 0x02};

CorsairLightingFirmware::CorsairLightingFirmware(corsair_product_enum_t product,
												 CorsairLightingFirmwareStorage* storage)
	: storage(storage), product(product) {
	storage->loadDeviceID(deviceId);
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
			response->send_P(pgm_read_word(&(firmwareVersions[product])), FIRMWARE_VERSION_SIZE);
			break;
		}
		case READ_DEVICE_ID: {
			response->send(deviceId.data, sizeof(deviceId));
			break;
		}
		case WRITE_DEVICE_ID: {
			DeviceID deviceID = {command.data[0], command.data[1], command.data[2], command.data[3]};
			setDeviceID(deviceID);
			response->send(deviceId.data, sizeof(deviceId));
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

void CorsairLightingFirmware::getDeviceID(DeviceID& deviceID) const { deviceID = deviceId; }

void CorsairLightingFirmware::setDeviceID(const DeviceID& deviceID) {
	deviceId = deviceID;
	storage->saveDeviceID(deviceID);
}

uint8_t CorsairLightingFirmware::getStatus() { return status; }

void CorsairLightingFirmware::setStatus(uint8_t a_status) { status = a_status; }

uint8_t CorsairLightingFirmware::getProduct() { return product; }
