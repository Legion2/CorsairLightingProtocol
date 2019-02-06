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
#ifndef _LEDCONTROLLER_h
#define _LEDCONTROLLER_h

#include "Arduino.h"

#include <FastLED.h>
#include <CorsairLightingProtocol.h>

#define CHANNEL_NUM 2
#define GROUPS_NUM 6

// iCUE changes to a very slow mode when there are more than 50 leds per channel
#define CHANNEL_LED_COUNT 50
//#define CHANNEL_LED_COUNT 60

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
	uint8_t brightness = CHANNEL_LED_BRIGHTNESS_MAX;
	uint8_t ledMode = CHANNEL_MODE_ON;
	uint8_t ledCount = 0;
	uint8_t ledPortType = PORT_TYPE_RGB_LED_STRIP;

	uint16_t temp;

	Group groups[GROUPS_NUM];
	uint8_t groupsSet = 0;
};

struct LEDBufferData {
	CRGB const* led_buffer;

	uint8_t values_buffer[3][CHANNEL_LED_COUNT];
};

class LEDController_ {
public:
	LEDController_();
	void addLeds(uint8_t channel, CRGB const* led_buffer);
	void handleLEDControl(const Command & command);
	bool updateLEDs();
protected:
	void addColors(CRGB * led_buffer, const CRGB& color, const uint8_t* values, uint8_t length);
	Channel channels[CHANNEL_NUM];
	bool trigger_update = false;
	bool trigger_save = false;

	LEDBufferData volatileData[CHANNEL_NUM];

	long lastUpdate = 0;
	long currentUpdate = 0;

	int applySpeed(int duration, byte speed);
	int animation_step(int duration, int steps);
	int animation_step_count(int duration, int steps);

	bool load();
	bool save();
};

extern LEDController_ LEDController;

#endif

