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
#include "HID-Project.h"

CorsairLightingFirmware_& CorsairLightingFirmware()
{
	static CorsairLightingFirmware_ obj;
	return obj;
}

void CorsairLightingFirmware_::handleFirmwareCommand(const Command & command)
{
	switch (command.command)
	{
	case READ_STATUS:
		CorsairLightingProtocol.response_P((uint8_t*)status, sizeof(status), 1);
		break;
	case READ_FIRMWARE_VERSION:
		CorsairLightingProtocol.response_P(firmware_version, sizeof(firmware_version), 1);
		break;
	case READ_DEVICE_ID:
		CorsairLightingProtocol.response(DeviceId, sizeof(DeviceId), 1);
		break;
	case WRITE_DEVICE_ID:
		memcpy(DeviceId, command.data, 4);
		CorsairLightingProtocol.response(DeviceId, sizeof(DeviceId), 1);
		break;
	case READ_BOOTLOADER_VERSION:
		CorsairLightingProtocol.response_P(bootloader_version, sizeof(bootloader_version), 1);
		break;
	}
}
