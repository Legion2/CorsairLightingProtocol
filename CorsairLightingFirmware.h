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

#include <FastLED.h>
// VID: 1b1c
// PID: 0c0b
// Revision: 0001
// Manufacturer: Corsair
// Product Name: Lighting Node PRO
// Serial Number: 1EA000060904BAAEFB66DF55421900F5

#define CHANNELS_NUM 2
#define GROUPS_NUM 1
//Channel modes
#define CHANNEL_MODE_DISABLED 0x00
#define CHANNEL_MODE_ON 0x01

//Port types
#define PORT_TYPE_DISABLED 0x00
#define PORT_TYPE_RGB_LED_STRIP 0x01
#define PORT_TYPE_RGB_LED_SP_FANS 0x02
#define PORT_TYPE_RGB_LED_LL_FANS 0x03

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

const uint8_t firmware_version[] PROGMEM = { 0x00, 0x02, 0x36 };
const uint8_t bootloader_version[] PROGMEM = { 0x00, 0x02 };
const char status[] PROGMEM = "test";

struct Group {
	byte ledIndex = 0;//start index of the leds of this group
	byte ledCount = 0;//number of leds in this group
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
	uint8_t brightness = 0x64;
	uint8_t ledMode = CHANNEL_MODE_ON;
	uint8_t ledCount = 0;
	uint8_t ledPortType = PORT_TYPE_RGB_LED_STRIP;

	uint16_t temp = 0;

	Group groups[GROUPS_NUM];
	uint8_t groupsSet = 0;
};

void handleCommand(const byte& command, const byte* data);

class CorsairLightingFirmware_ {
public:
	void handleFirmwareCommand(const byte& command, const byte* data);
protected:
	uint8_t DeviceId[4] = { 0x01, 0x00, 0x00, 0x00 };
};

CorsairLightingFirmware_& CorsairLightingFirmware();

class LEDController_ {
public:
	void handleLEDControl(const byte & command, const byte * data);
protected:
	Channel channels[CHANNELS_NUM];
};

LEDController_& LEDController();

#endif

