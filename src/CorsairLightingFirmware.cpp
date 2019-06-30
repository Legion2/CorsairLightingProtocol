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
#include <EEPROM.h>
#include "CorsairLightingFirmware.h"

CorsairLightingFirmware::CorsairLightingFirmware(const uint8_t* firmwareVersion) : firmwareVersion(firmwareVersion)
{
	EEPROM.get(EEPROM_ADDRESS_DEVICE_ID, DeviceId);
}

void CorsairLightingFirmware::handleFirmwareCommand(const Command & command, const CorsairLightingProtocolResponse* response)
{
	switch (command.command)
	{
	case READ_STATUS:
	{
		uint8_t statusData[] = { PROTOCOL_STATUS_OK };
		response->send(statusData, sizeof(statusData));
		break;
	}
	case READ_FIRMWARE_VERSION:
	{
		response->send_P(firmwareVersion, FIRMWARE_VERSION_SIZE);
		break;
	}
	case READ_DEVICE_ID:
	{
		response->send(DeviceId, sizeof(DeviceId));
		break;
	}
	case WRITE_DEVICE_ID:
	{
		memcpy(DeviceId, command.data, sizeof(DeviceId));
		EEPROM.put(EEPROM_ADDRESS_DEVICE_ID, DeviceId);
		response->send(DeviceId, sizeof(DeviceId));
		break;
	}
	case READ_BOOTLOADER_VERSION:
	{
		response->send_P(bootloader_version, sizeof(bootloader_version));
		break;
	}
	}
}
