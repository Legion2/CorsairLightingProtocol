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
#include "LEDController.h"
#include "TemperatureController.h"

#ifndef EEPROM_ADDRESS
#define EEPROM_ADDRESS 4
#endif

class FastLEDController : public LEDController {
	struct LEDBufferData {
		uint8_t ledCount = 0;
		CRGB * led_buffer = nullptr;
		// store an array for each color
		uint8_t *values_buffer[3] = { nullptr };
		// current temperature
		uint16_t temp;
	};

public:
	FastLEDController(bool useEEPROM);
	FastLEDController(TemperatureController* temperatureController, bool useEEPROM);
	~FastLEDController();
	virtual void addLeds(uint8_t channel, CRGB * led_buffer, uint8_t count);
	CRGB* getLeds(uint8_t channel);
	uint8_t getLedCount(uint8_t channel);
	virtual bool updateLEDs();
	virtual size_t getEEPROMSize();
protected:
	TemperatureController* const temperatureController;

	bool trigger_update = false;

	LEDBufferData volatileData[CHANNEL_NUM];

	long lastUpdate = 0;
	long currentUpdate = 0;

	int applySpeed(int duration, byte speed);
	int animation_step(int duration, int steps);
	int animation_step_count(int duration, int steps);
	void addColors(CRGB* led_buffer, const CRGB& color, const uint8_t* values, uint8_t length);

	const bool useEEPROM;
	bool load() override;
	bool save() override;

	virtual void triggerLEDUpdate() override;
	virtual void setLEDExternalTemperature(uint8_t channel, uint16_t temp) override;
	virtual void setLEDColorValues(uint8_t channel, uint8_t color, uint8_t offset, const uint8_t* values, size_t len) override;
	virtual void clearLEDColorValues(uint8_t channel) override;
};
