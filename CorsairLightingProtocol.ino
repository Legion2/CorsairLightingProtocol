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
#include <FastLED.h>
#include "HID-Project.h"

// VID: 1b1c
// PID: 0c0b
// Revision: 0001
// Manufacturer: Corsair
// Product Name: Lighting Node PRO
// Serial Number: 1EA000060904BAAEFB66DF55421900F5

#define CHANNELS_NUM 2
#define GROUPS_NUM 1

//strip/fan number
#define STRIP_SETUP_MASK_DISABLED 0x00
#define STRIP_SETUP_MASK_1 0x01
#define STRIP_SETUP_MASK_2 0x02
#define STRIP_SETUP_MASK_3 0x03

//strip/fan number
#define STRIP_1 0x00
#define STRIP_2 0x0A
#define STRIP_3 0x14
#define STRIP_4 0x1E

//LED type
#define TYPE_RGB_LED_STRIP  0x0A
#define TYPE_RGB_HD_FAN	    0x0C
#define TYPE_RGB_SP_FAN	    0x01
#define TYPE_RGB_ML_FAN	    0x04
//LED group mode
#define GROUP_MODE_Rainbow_Wave 0x00
#define GROUP_MODE_Color_Shift 0x01
#define GROUP_MODE_Color_Pulse 0x02
#define GROUP_MODE_Color_Wave 0x03
#define GROUP_MODE_Static 0x04
#define GROUP_MODE_Temperature 0x05
#define GROUP_MODE_Visor 0x06
#define GROUP_MODE_Marquee 0x07
#define GROUP_MODE_Blink 0x08
#define GROUP_MODE_Sequential 0x09
#define GROUP_MODE_Rainbow 0x0A

//LED group speed
#define GROUP_SPEED_HIGH   0x00
#define GROUP_SPEED_MEDIUM 0x01
#define GROUP_SPEED_LOW    0x02

//LED group direction
#define GROUP_DIRECTION_BACKWARD 0x00
#define GROUP_DIRECTION_FORWARD  0x01

const uint8_t firmware_version[] PROGMEM = { 0x00, 0x00, 0x02, 0x36 };
const uint8_t bootloader_version[] PROGMEM = { 0x00, 0x02 };

uint8_t rawhidData[64];

uint8_t DeviceId[4] = { 0x01, 0x00, 0x00, 0x00 };

struct Group {
	byte type = TYPE_RGB_LED_STRIP;
	byte mode = GROUP_MODE_Rainbow_Wave;
	byte speed = GROUP_SPEED_HIGH;
	byte direction = GROUP_DIRECTION_FORWARD;
	byte extra = 0x00;

	CRGB color1;
	CRGB color2;
	CRGB color3;

	uint16_t temp1;
	uint16_t temp2;
	uint16_t temp3;
};

struct Channel {
	uint8_t brightness = 0;
	uint8_t ledMode = 0;
	uint8_t ledCount = 0;
	uint8_t ledPortType = 0;

	uint16_t temp = 0;

	Group groups[GROUPS_NUM];
};

Channel channels[CHANNELS_NUM];

void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);
	RawHID.begin(rawhidData, sizeof(rawhidData));
}

void loop() {
	// Check if there is new data from the RawHID device
	auto bytesAvailable = RawHID.available();
	if (bytesAvailable)
	{
		if (bytesAvailable != 16) {
			Serial.print(F("bytesAvailable: "));
			Serial.print(bytesAvailable);
			Serial.print("\n");
			return;
		}
		int command = RawHID.read();
		uint8_t buffer[15];
		RawHID.readBytes(buffer, sizeof(buffer));
		doCommand(command, buffer);
	}
}

void doCommand(byte command, byte* data) {
	if (command >= 0x10 && command < 0x30) {
		response(0);
		Serial.print(F("ignore: "));
		Serial.print(command, HEX);
		Serial.print("\n");
	}
	else if (command >= 0x30 && command < 0x40) {
		ledControll(command, data);
	}
	else {
		switch (command)
		{
		case 0x01://ReadStatus
			response((uint8_t*) "test", 4);
			break;
		case 0x02://ReadFirmwareVersion
			response_P(firmware_version, sizeof(firmware_version));
			break;
		case 0x03://ReadDeviceId
			response(DeviceId, sizeof(DeviceId));
			break;
		case 0x04://WriteDeviceId
			memcpy(DeviceId, data, 4);
			response(DeviceId, sizeof(DeviceId));
			break;
		case 0x06://ReadBootloaderVersion
			response_P(bootloader_version, sizeof(bootloader_version));
			break;
		}
	}
}
void response(const uint8_t data) {
	response(&data, 1);
}

void response(const uint8_t* data, size_t size) {
	uint8_t response[16];
	memset(response, 0x00, sizeof(response));
	if (size > sizeof(response)) {
		return;
	}
	memcpy(response, data, size);
	RawHID.write(response, sizeof(response));
}

void response_P(const uint8_t* data, size_t size) {
	uint8_t response[16];
	memset(response, 0x00, sizeof(response));
	if (size > sizeof(response)) {
		return;
	}
	memcpy_P(response, data, size);
	RawHID.write(response, sizeof(response));
}


void ledControll(byte command, byte* data) {
#define ledChannel data[0]
	Serial.print(F("led channel: "));
	Serial.print(ledChannel, DEC);
	Serial.print("\n");
	switch (command)
	{
	case 0x30://ReadLedStripMask
	{
		uint8_t ledMask[7];
		ledMask[0] = 0;//always zero
		ledMask[1] = 0x02;
		ledMask[2] = 0x01;
		ledMask[3] = 0x03;
		ledMask[4] = 0x01;
		ledMask[5] = 0x01;
		ledMask[6] = 0x01;
		response(ledMask, sizeof(ledMask));
		return;
		break;
	}
	case 0x31://WriteLedRgbValue
		Serial.println(F("WriteLedRgbValue"));
		break;
	case 0x33://commit
		Serial.println(F("commit"));
		break;
	case 0x34://WriteLedClear
		Serial.println(F("WriteLedClear"));
		break;
	case 0x35://WriteLedGroupSet
	{
		Group& group = channels[ledChannel].groups[0];
		group.type = data[2];
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
		channels[data[0]].temp = combine(data[2], data[3]);
		break;
	}
	case 0x37://WriteLedGroupsClear
		Serial.println(F("WriteLedGroupsClear"));
		break;
	case 0x38://WriteLedMode
	{
		Serial.print(F("mode: "));
		Serial.print(data[1], HEX);
		Serial.print("\n");
		channels[ledChannel].ledMode = data[1];
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
		channels[ledChannel].brightness = data[1];
		break;
	}
	case 0x3B://WriteLedPortType
	{
		Serial.print(F("ledPortType: "));
		Serial.print(data[1], HEX);
		Serial.print("\n");
		channels[ledChannel].ledPortType = data[1];
		break;
	}
	default:
		Serial.print(F("unkown command: "));
		Serial.print(command, HEX);
		Serial.print("\n");
		break;
	}
	response(0x00);
}

uint16_t combine(const byte& byte1, const byte& byte2) {
	uint16_t t = byte1;
	t = t << 8;
	t |= byte2;
	return t;
}

