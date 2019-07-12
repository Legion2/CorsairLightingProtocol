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
#include <FastLED.h>
#include "ILEDController.h"
#include "CLPUtils.h"

#define CHANNEL_NUM 2
#define GROUPS_NUM 6

//LED brightness
#define CHANNEL_LED_BRIGHTNESS_MIN 0
#define CHANNEL_LED_BRIGHTNESS_MAX 100

//Channel modes
#define CHANNEL_MODE_DISABLED 0x00
#define CHANNEL_MODE_ON 0x01
#define CHANNEL_MODE_SOFTWARE_PLAYBACK 0x02

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

//LED group extra
#define GROUP_EXTRA_ALTERNATING 0x00
#define GROUP_EXTRA_RANDOM  0x01

#define GROUP_TEMP_GROUP_EXTERNAL 255

struct LEDGroup {
	byte ledIndex = 0;//start index of the leds of this group
	byte ledCount = 0;//number of leds in this group
	byte mode = GROUP_MODE_Rainbow_Wave;
	byte speed = GROUP_SPEED_HIGH;
	byte direction = GROUP_DIRECTION_FORWARD;
	byte extra = 0x00;
	byte tempGroup = GROUP_TEMP_GROUP_EXTERNAL;

	CRGB color1;
	CRGB color2;
	CRGB color3;

	uint16_t temp1;
	uint16_t temp2;
	uint16_t temp3;
};

struct LEDChannel {
	uint8_t brightness = CHANNEL_LED_BRIGHTNESS_MAX;
	uint8_t ledMode = CHANNEL_MODE_ON;
	uint8_t ledCount = 0;
	uint8_t ledPortType = PORT_TYPE_RGB_LED_STRIP;

	LEDGroup groups[GROUPS_NUM];
	uint8_t groupsSet = 0;
};

class LEDController : public ILEDController {
public:
	virtual void handleLEDControl(const Command & command, const CorsairLightingProtocolResponse* response) override;
protected:
	LEDChannel channels[CHANNEL_NUM];
	// Indicates that the configuration of the channels has been changed and should be saved
	bool trigger_save = false;

	//Trigger update of the LEDs
	virtual void triggerLEDUpdate() = 0;
	// The led count of the group
	virtual uint8_t getLEDStripMask(uint8_t channel, uint8_t set);
	// The temperature in hundredths of a degree Celsius.
	virtual void setLEDExternalTemperature(uint8_t channel, uint16_t temp) = 0;
	virtual bool setLEDGroup(uint8_t channel, uint8_t set, LEDGroup& group);
	virtual void setLEDColorValues(uint8_t channel, uint8_t color, uint8_t offset, const uint8_t* values, size_t len) = 0;
	virtual bool setLEDMode(uint8_t channel, uint8_t mode);
	virtual bool setLEDBrightness(uint8_t channel, uint8_t brightness);
	virtual bool setLEDPortType(uint8_t channel, uint8_t ledPortType);
	virtual void clearLEDColorValues(uint8_t channel) = 0;
	virtual bool clearLEDGroups(uint8_t channel);
	virtual bool save() = 0;
	virtual bool load() = 0;
};
