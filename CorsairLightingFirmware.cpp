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

LEDController_& LEDController()
{
	static LEDController_ obj;
	return obj;
}

uint16_t combine(const byte& byte1, const byte& byte2) {
	uint16_t t = byte1;
	t = t << 8;
	t |= byte2;
	return t;
}

void CorsairLightingFirmware_::handleFirmwareCommand(const Command & command)
{
	switch (command.command)
	{
	case 0x01://ReadStatus
		CorsairLightingProtocol.response_P((uint8_t*) status, sizeof(status), 1);
		break;
	case 0x02://ReadFirmwareVersion
		CorsairLightingProtocol.response_P(firmware_version, sizeof(firmware_version), 1);
		break;
	case 0x03://ReadDeviceId
		CorsairLightingProtocol.response(DeviceId, sizeof(DeviceId), 1);
		break;
	case 0x04://WriteDeviceId
		memcpy(DeviceId, command.data, 4);
		CorsairLightingProtocol.response(DeviceId, sizeof(DeviceId), 1);
		break;
	case 0x06://ReadBootloaderVersion
		CorsairLightingProtocol.response_P(bootloader_version, sizeof(bootloader_version), 1);
		break;
	}
}

void LEDController_::handleLEDControl(const Command& command) {
	auto& data = command.data;
	if (data[0] < sizeof(channels)) {
#define ledChannel channels[data[0]]
		switch (command.command)
		{
		case 0x30://ReadLedStripMask
		{
			uint8_t ledMask[GROUPS_NUM];
			for (unsigned int i = 0; i < GROUPS_NUM; i++) {
				if (i < ledChannel.groupsSet) {
					ledMask[i] = ledChannel.groups[i].ledCount;
				}
				else {
					ledMask[i] = 0x00;
				}
			}
			CorsairLightingProtocol.response(ledMask, sizeof(ledMask), 1);
			return;
			break;
		}
		case 0x31://WriteLedRgbValue
#ifdef DEBUG
			Serial.println(F("WriteLedRgbValue"));
#endif
			break;
		case 0x33://commit
#ifdef DEBUG
			Serial.println(F("commit"));
#endif
			break;
		case 0x34://WriteLedClear
#ifdef DEBUG
			Serial.println(F("WriteLedClear"));
#endif
			break;
		case 0x35://WriteLedGroupSet
		{
#ifdef DEBUG
			Serial.println(F("WriteLedGroupSet"));
#endif
			if (ledChannel.groupsSet >= GROUPS_NUM) {
#ifdef DEBUG
				Serial.print(F("max groups: "));
				Serial.print(GROUPS_NUM, HEX);
				Serial.print("\n");
#endif
				break;
			}
			Group& group = ledChannel.groups[ledChannel.groupsSet++];
			group.ledIndex = data[1];
			group.ledCount = data[2];
			group.mode = data[3];
			group.speed = data[4];
			group.direction = data[5];
			group.extra = data[6];
			// byte 7 is 0xFF
			group.color1.setRGB(data[8], data[9], data[10]);
			group.color2.setRGB(data[11], data[12], data[13]);
			group.color3.setRGB(data[14], data[15], data[16]);
			group.temp1 = combine(data[17], data[18]);
			group.temp2 = combine(data[19], data[20]);
			group.temp3 = combine(data[21], data[22]);
			break;
		}
		case 0x36://WriteLedExternalTemp
		{
			ledChannel.temp = combine(data[2], data[3]);
			break;
		}
		case 0x37://WriteLedGroupsClear
		{
#ifdef DEBUG
			Serial.println(F("WriteLedGroupsClear"));
#endif
			ledChannel.groupsSet = 0;
			break;
		}
		case 0x38://WriteLedMode
		{
#ifdef DEBUG
			Serial.print(F("mode: "));
			Serial.print(data[1], HEX);
			Serial.print("\n");
#endif
			ledChannel.ledMode = data[1];
			break;
		}
		case 0x39://WriteLedBrightness
		{
			/*Byte 2 is the brightness where
				0x00 = > 0 %
				0x21 = > 33 %
				0x42 = > 66 %
				0x64 = > 100 %
				*/
			ledChannel.brightness = data[1];
			break;
		}
		case 0x3A://WriteLedCount 
		{
			ledChannel.ledCount = data[1];
		}
		case 0x3B://WriteLedPortType
		{
#ifdef DEBUG
			Serial.print(F("ledPortType: "));
			Serial.print(data[1], HEX);
			Serial.print("\n");
#endif
			ledChannel.ledPortType = data[1];
			break;
		}
		default:
#ifdef DEBUG
			Serial.print(F("unkown command: "));
			Serial.print(command.command, HEX);
			Serial.print("\n");
#endif
			break;
		}
	}
	CorsairLightingProtocol.response(0x00);
}