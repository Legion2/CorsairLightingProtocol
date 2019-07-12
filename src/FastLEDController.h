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

template<size_t CHANNEL_LED_COUNT>
class FastLEDController : public LEDController {
	struct LEDBufferData {
		CRGB * led_buffer;
		// store an array for each color
		uint8_t values_buffer[3][CHANNEL_LED_COUNT];
		// current temperature
		uint16_t temp;
	};

public:
	FastLEDController(bool useEEPROM);
	FastLEDController(TemperatureController* temperatureController, bool useEEPROM);
	virtual void addLeds(uint8_t channel, CRGB * led_buffer);
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

#include <EEPROM.h>

#ifndef EEPROM_ADDRESS
#define EEPROM_ADDRESS 4
#endif

template<size_t CHANNEL_LED_COUNT>
FastLEDController<CHANNEL_LED_COUNT>::FastLEDController(bool useEEPROM) : temperatureController(NULL), useEEPROM(useEEPROM) {
	load();
}

template<size_t CHANNEL_LED_COUNT>
inline FastLEDController<CHANNEL_LED_COUNT>::FastLEDController(TemperatureController* temperatureController, bool useEEPROM) : temperatureController(temperatureController), useEEPROM(useEEPROM)
{
	load();
}

template<size_t CHANNEL_LED_COUNT>
void FastLEDController<CHANNEL_LED_COUNT>::addLeds(uint8_t channel, CRGB * led_buffer) {
	volatileData[channel].led_buffer = led_buffer;
}

template<size_t CHANNEL_LED_COUNT>
void FastLEDController<CHANNEL_LED_COUNT>::addColors(CRGB* led_buffer, const CRGB& color, const uint8_t* values, uint8_t length) {
	for (int i = 0; i < length; i++) {
		led_buffer[i] += color % values[i];
	}
}

template<size_t CHANNEL_LED_COUNT>
int FastLEDController<CHANNEL_LED_COUNT>::applySpeed(int duration, byte speed) {
	switch (speed)
	{
	case GROUP_SPEED_HIGH:
		return duration / 2;
	case GROUP_SPEED_MEDIUM:
		return duration;
	case GROUP_SPEED_LOW:
		return duration * 2;
	default:
		return duration;
	}
}

/*
returns the current step of the animation
*/
template<size_t CHANNEL_LED_COUNT>
int FastLEDController<CHANNEL_LED_COUNT>::animation_step(int duration, int steps) {
	int currentStep = ((currentUpdate % duration) / ((float)duration)) * steps;
	return currentStep;
}
/*
returns the number of steps since the last update
*/
template<size_t CHANNEL_LED_COUNT>
int FastLEDController<CHANNEL_LED_COUNT>::animation_step_count(int duration, int steps) {
	long lastAnimationNumber = lastUpdate / duration;
	long currentAnimationNumber = currentUpdate / duration;
	int lastStep = ((lastUpdate % duration) / ((float)duration)) * steps;
	int currentStep = ((currentUpdate % duration) / ((float)duration)) * steps;

	return currentStep - lastStep + (currentAnimationNumber - lastAnimationNumber) * steps;
}

template<size_t CHANNEL_LED_COUNT>
bool FastLEDController<CHANNEL_LED_COUNT>::updateLEDs()
{
	lastUpdate = currentUpdate;
	currentUpdate = millis();

	bool updated = false;

	for (int channelId = 0; channelId < CHANNEL_NUM; channelId++) {
		LEDChannel& channel = channels[channelId];
		switch (channel.ledMode)
		{
		case CHANNEL_MODE_DISABLED:
		{
			break;
		}
		case CHANNEL_MODE_ON:
		{
			for (int i = 0; i < channel.groupsSet; i++) {
				LEDGroup& group = channel.groups[i];
				switch (group.mode)
				{
				case GROUP_MODE_Rainbow_Wave:
				{
					int duration = applySpeed(3300, group.speed);
					int count = animation_step_count(duration, 256);
					if (count > 0) {
						int step = animation_step(duration, 256);
						int move = group.direction == GROUP_DIRECTION_FORWARD ? -15 : 15;
						for (int i = 0; i < group.ledCount; i++) {
							volatileData[channelId].led_buffer[group.ledIndex + i] = CHSV(step + (i * move), 255, 255) % channel.brightness;
						}
						updated = true;
					}
					break;
				}
				case GROUP_MODE_Color_Shift:
				{
					int duration = applySpeed(3000, group.speed);
					int count = animation_step_count(duration, 512);
					if (count > 0) {
						int step = animation_step(duration, 512);
						if (count > step) {
							if (group.extra == GROUP_EXTRA_RANDOM) {
								group.color1 = group.color2;
								group.color2 = CHSV(random8(), 255, 255);
							}
							else if (group.extra == GROUP_EXTRA_ALTERNATING) {
								group.color3 = group.color1;
								group.color1 = group.color2;
								group.color2 = group.color3;
							}
						}
						uint8_t scale;
						if (step < 128) {
							scale = 0;
						}
						else if (step < 384) {
							scale = ease8InOutApprox(step - 128);
						}
						else {
							scale = 255;
						}

						fill_solid(&volatileData[channelId].led_buffer[group.ledIndex], group.ledCount, group.color1.lerp8(group.color2, scale) % channel.brightness);
						updated = true;
					}
					break;
				}
				case GROUP_MODE_Color_Pulse:
				{
					int duration = applySpeed(3000, group.speed);
					int count = animation_step_count(duration, 512);
					if (count > 0) {
						int step = animation_step(duration, 512);
						if (count > step) {
							if (group.extra == GROUP_EXTRA_RANDOM) {
								group.color1 = CHSV(random8(), 255, 255);
							}
							else if (group.extra == GROUP_EXTRA_ALTERNATING) {
								group.color3 = group.color1;
								group.color1 = group.color2;
								group.color2 = group.color3;
							}
						}
						uint8_t scale = ease8InOutApprox((uint8_t)step);
						if (step >= 256) {
							scale = 255 - scale;
						}

						fill_solid(&volatileData[channelId].led_buffer[group.ledIndex], group.ledCount, group.color1 % scale % channel.brightness);
						updated = true;
					}
					break;
				}
				case GROUP_MODE_Color_Wave:
				{
					int duration = applySpeed(3000, group.speed);
					int count = animation_step_count(duration, 10000);
					if (count > 0) {
						int step = animation_step(duration, 10000);
						if (count > step) {
							if (group.extra == GROUP_EXTRA_RANDOM) {
								group.color1 = group.color2;
								group.color2 = CHSV(random8(), 255, 255);
							}
							else if (group.extra == GROUP_EXTRA_ALTERNATING) {
								group.color3 = group.color1;
								group.color1 = group.color2;
								group.color2 = group.color3;
							}
						}
						float valley = step / 10000.0;
						for (int i = 0; i < group.ledCount; i++) {
							float pos = (i % 17) / 17.0;

							float distanceWave;
							CRGB color;
							const bool flag = (i % 34) < 17;
							if (pos < valley) {
								color = flag ? group.color1 : group.color2;
								distanceWave = abs(valley - 0.5 - pos);
							}
							else {
								color = flag ? group.color2 : group.color1;
								distanceWave = abs(valley + 0.5 - pos);
							}

							uint8_t scale;
							if (distanceWave > 0.25) {
								scale = 0;
							}
							else {
								scale = 255 - ease8InOutApprox((distanceWave * 4) * 256);
							}
							volatileData[channelId].led_buffer[group.ledIndex + i] = (color % scale) % channel.brightness;
						}
						updated = true;
					}
					break;
				}
				case GROUP_MODE_Static:
				{
					fill_solid(&volatileData[channelId].led_buffer[group.ledIndex], group.ledCount, group.color1 % channel.brightness);
					break;
				}
				case GROUP_MODE_Temperature:
				{
					uint16_t currentTemperature;
					const uint8_t& tempGroup = group.tempGroup;
					if (tempGroup == GROUP_TEMP_GROUP_EXTERNAL) {
						currentTemperature = volatileData[channelId].temp;
					}
					else if (tempGroup < TEMPERATURE_NUM && temperatureController != NULL) {
						currentTemperature = temperatureController->getTemperature(tempGroup);
					}

					CRGB color;
					if (currentTemperature < group.temp1) {
						color = group.color1;
					}
					else if (currentTemperature < group.temp2) {
						color = group.color1.lerp16(group.color2, ((currentTemperature - group.temp1) / ((float)(group.temp2 - group.temp1))) * 65535);
					}
					else if (currentTemperature < group.temp3) {
						color = group.color2.lerp16(group.color3, ((currentTemperature - group.temp2) / ((float)(group.temp3 - group.temp2))) * 65535);
					}
					else {
						color = group.color3;
					}

					fill_solid(&volatileData[channelId].led_buffer[group.ledIndex], group.ledCount, color % channel.brightness);
					updated = true;
					break;
				}
				case GROUP_MODE_Visor:
				{
					int duration = applySpeed(150 * group.ledCount, group.speed);
					int steps = group.ledCount * 2;
					int count = animation_step_count(duration, steps);
					if (count > 0) {
						int step = animation_step(duration, steps);
						if (step >= group.ledCount ? count > step - group.ledCount : count > step) {
							if (group.extra == GROUP_EXTRA_RANDOM) {
								group.color1 = CHSV(random8(), 255, 255);
							}
							else if (group.extra == GROUP_EXTRA_ALTERNATING) {
								group.color3 = group.color1;
								group.color1 = group.color2;
								group.color2 = group.color3;
							}
						}
						fill_solid(&volatileData[channelId].led_buffer[group.ledIndex], group.ledCount, CRGB::Black);
						for (int i = 0; i < 4; i++) {
							int led = (((step - i) % steps) + steps) % steps;
							if (led >= group.ledCount) {
								led = steps - led - 1;
							}
							volatileData[channelId].led_buffer[group.ledIndex + led] = group.color1 % channel.brightness;
						}
						updated = true;
					}
					break;
				}
				case GROUP_MODE_Marquee:
				{
					int duration = applySpeed(700, group.speed);
					int count = animation_step_count(duration, 3);
					if (count > 0) {
						int step = animation_step(duration, 3);
						for (int i = 0; i < group.ledCount; i++) {
							volatileData[channelId].led_buffer[group.ledIndex + i] = (i + step) % 3 > 0 ? group.color1 % channel.brightness : CRGB::Black;
						}
						updated = true;
					}
					break;
				}
				case GROUP_MODE_Blink:
				{
					int duration = applySpeed(3000, group.speed);
					int count = animation_step_count(duration, 2);
					if (count > 0) {
						int step = animation_step(duration, 2);
						if (count > step) {
							if (group.extra == GROUP_EXTRA_RANDOM) {
								group.color1 = CHSV(random8(), 255, 255);
							}
							else if (group.extra == GROUP_EXTRA_ALTERNATING) {
								group.color3 = group.color1;
								group.color1 = group.color2;
								group.color2 = group.color3;
							}
						}

						fill_solid(&volatileData[channelId].led_buffer[group.ledIndex], group.ledCount, step == 0 ? group.color1 % channel.brightness : CRGB::Black);
						updated = true;
					}
					break;
				}
				case GROUP_MODE_Sequential:
				{
					int steps = group.ledCount;
					int duration = applySpeed(60 * steps, group.speed);
					int count = animation_step_count(duration, steps);
					if (count > 0) {
						int step = animation_step(duration, steps);
						if (count > step) {
							if (group.extra == GROUP_EXTRA_RANDOM) {
								group.color2 = group.color1;
								group.color1 = CHSV(random8(), 255, 255);
							}
						}

						if (group.direction == GROUP_DIRECTION_FORWARD) {
							fill_solid(&volatileData[channelId].led_buffer[group.ledIndex], step + 1, group.color1 % channel.brightness);
							fill_solid(&volatileData[channelId].led_buffer[group.ledIndex + step + 1], group.ledCount - (step + 1), group.color2 % channel.brightness);
						}
						else {
							fill_solid(&volatileData[channelId].led_buffer[group.ledIndex + group.ledCount - (step + 1)], step + 1, group.color1 % channel.brightness);
							fill_solid(&volatileData[channelId].led_buffer[group.ledIndex], group.ledCount - (step + 1), group.color2 % channel.brightness);
						}
						updated = true;
					}
					break;
				}
				case GROUP_MODE_Rainbow:
				{
					int duration = applySpeed(3000, group.speed);
					int count = animation_step_count(duration, 256);
					if (count > 0) {
						int step = animation_step(duration, 256);
						fill_solid(&volatileData[channelId].led_buffer[group.ledIndex], group.ledCount, CHSV(step, 255, 255) % channel.brightness);
						updated = true;
					}
					break;
				}
				default:
				{
#ifdef DEBUG
					Serial.print(F("unkown group mode: "));
					Serial.print(group.mode, HEX);
					Serial.println();
#endif
					break;
				}
				}
			}
			updated = true;
			break;
		}
		case CHANNEL_MODE_SOFTWARE_PLAYBACK:
		{
			if (trigger_update) {
				auto& data = volatileData[channelId];
				fill_solid(data.led_buffer, CHANNEL_LED_COUNT, CRGB::Black);
				addColors(data.led_buffer, CRGB::Red, data.values_buffer[0], CHANNEL_LED_COUNT);
				addColors(data.led_buffer, CRGB::Green, data.values_buffer[1], CHANNEL_LED_COUNT);
				addColors(data.led_buffer, CRGB::Blue, data.values_buffer[2], CHANNEL_LED_COUNT);
				updated = true;
			}
			break;
		}
		default:
		{
#ifdef DEBUG
			Serial.print(F("unkown led channel mode: "));
			Serial.print(channel.ledMode, HEX);
			Serial.println();
			break;
#endif
		}
		}
	}
	trigger_update = false;
	return updated;
}

template<size_t CHANNEL_LED_COUNT>
inline size_t FastLEDController<CHANNEL_LED_COUNT>::getEEPROMSize()
{
	return sizeof(channels);
}

template<size_t CHANNEL_LED_COUNT>
bool FastLEDController<CHANNEL_LED_COUNT>::load() {
	if (useEEPROM) {
		EEPROM.get(EEPROM_ADDRESS, channels);
		return true;
	}
	return false;
}

template<size_t CHANNEL_LED_COUNT>
bool FastLEDController<CHANNEL_LED_COUNT>::save() {
	if (useEEPROM) {
#ifdef DEBUG
		Serial.println(F("Save to EEPROM."));
#endif
		EEPROM.put(EEPROM_ADDRESS, channels);
		return true;
	}
	return false;
}

template<size_t CHANNEL_LED_COUNT>
inline void FastLEDController<CHANNEL_LED_COUNT>::triggerLEDUpdate()
{
	trigger_update = true;
}

template<size_t CHANNEL_LED_COUNT>
inline void FastLEDController<CHANNEL_LED_COUNT>::setLEDExternalTemperature(uint8_t channel, uint16_t temp)
{
	volatileData[channel].temp = temp;
}

template<size_t CHANNEL_LED_COUNT>
inline void FastLEDController<CHANNEL_LED_COUNT>::setLEDColorValues(uint8_t channel, uint8_t color, uint8_t offset, const uint8_t* values, size_t len)
{
	size_t copyLength = min(CHANNEL_LED_COUNT - offset, len);
	if (copyLength >= 0) {
		memcpy(volatileData[channel].values_buffer[color] + offset, values, copyLength);
	}
}

template<size_t CHANNEL_LED_COUNT>
inline void FastLEDController<CHANNEL_LED_COUNT>::clearLEDColorValues(uint8_t channel)
{
	memset(volatileData[channel].values_buffer, 0, sizeof(volatileData[channel].values_buffer));
}
