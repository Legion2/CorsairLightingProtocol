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
#include "FastLEDController.h"
#include <EEPROM.h>

FastLEDController::FastLEDController(bool useEEPROM) : temperatureController(nullptr), useEEPROM(useEEPROM)
{
	load();
}

FastLEDController::FastLEDController(TemperatureController* temperatureController, bool useEEPROM) : temperatureController(temperatureController), useEEPROM(useEEPROM)
{
	load();
}

FastLEDController::~FastLEDController()
{
	for (auto& data : channelData) {
		for (uint8_t*& buffer : data.valuesBuffer) {
			delete[] buffer;
		}
	}
}

void FastLEDController::addLEDs(uint8_t channel, CRGB* leds, uint8_t count) {
	if (channel >= CHANNEL_NUM || leds == nullptr || channelData[channel].leds != nullptr) {
		return;
	}
	channelData[channel].ledCount = count;
	channelData[channel].leds = leds;
	for (uint8_t*& buffer : channelData[channel].valuesBuffer) {
		buffer = new uint8_t[count];
	}
}

CRGB* FastLEDController::getLEDs(uint8_t channel)
{
	if (channel >= CHANNEL_NUM) {
		return nullptr;
	}
	return channelData[channel].leds;
}

uint8_t FastLEDController::getLEDCount(uint8_t channel)
{
	if (channel >= CHANNEL_NUM) {
		return 0;
	}
	return channelData[channel].ledCount;
}

int FastLEDController::applySpeed(int duration, byte speed) {
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

int FastLEDController::animation_step(int duration, int steps) {
	int currentStep = ((currentUpdate % duration) / ((float)duration)) * steps;
	return currentStep;
}

int FastLEDController::animation_step_count(int duration, int steps) {
	long lastAnimationNumber = lastUpdate / duration;
	long currentAnimationNumber = currentUpdate / duration;
	int lastStep = ((lastUpdate % duration) / ((float)duration)) * steps;
	int currentStep = ((currentUpdate % duration) / ((float)duration)) * steps;

	return currentStep - lastStep + (currentAnimationNumber - lastAnimationNumber) * steps;
}

bool FastLEDController::updateLEDs()
{
	lastUpdate = currentUpdate;
	currentUpdate = millis();

	bool anyUpdate = false;

	for (int channelId = 0; channelId < CHANNEL_NUM; channelId++) {
		if (channelData[channelId].leds == nullptr) {
			continue;
		}
		bool updated = false;
		LEDChannel& channel = channels[channelId];

		switch (channel.ledMode)
		{
		case CHANNEL_MODE_DISABLED:
		{
			break;
		}
		case CHANNEL_MODE_ON:
		{
			for (uint8_t groupIndex = 0; groupIndex < channel.groupsSet; groupIndex++) {
				LEDGroup& group = channel.groups[groupIndex];
				int groupLedCount = min((int)channelData[channelId].ledCount - group.ledIndex, (int)group.ledCount);
				if (groupLedCount <= 0) {
					continue;
				}

				switch (group.mode)
				{
				case GROUP_MODE_Rainbow_Wave:
				{
					int duration = applySpeed(3300, group.speed);
					int count = animation_step_count(duration, 256);
					if (count > 0) {
						int step = animation_step(duration, 256);
						int move = group.direction == GROUP_DIRECTION_FORWARD ? -3 : 3;
						for (int i = 0; i < groupLedCount; i++) {
							channelData[channelId].leds[group.ledIndex + i] = CHSV(step + (i * move), 255, 255);
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

						fill_solid(&channelData[channelId].leds[group.ledIndex], groupLedCount, group.color1.lerp8(group.color2, scale));
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

						fill_solid(&channelData[channelId].leds[group.ledIndex], groupLedCount, group.color1 % scale);
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
						for (int i = 0; i < groupLedCount; i++) {
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
							channelData[channelId].leds[group.ledIndex + i] = (color % scale);
						}
						updated = true;
					}
					break;
				}
				case GROUP_MODE_Static:
				{
					fill_solid(&channelData[channelId].leds[group.ledIndex], groupLedCount, group.color1);
					updated = true;
					break;
				}
				case GROUP_MODE_Temperature:
				{
					uint16_t currentTemperature;
					const uint8_t& tempGroup = group.tempGroup;
					if (tempGroup == GROUP_TEMP_GROUP_EXTERNAL) {
						currentTemperature = channelData[channelId].temp;
					}
					else if (tempGroup < TEMPERATURE_NUM && temperatureController != nullptr) {
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

					fill_solid(&channelData[channelId].leds[group.ledIndex], groupLedCount, color);
					updated = true;
					break;
				}
				case GROUP_MODE_Visor:
				{
					int duration = applySpeed(150 * groupLedCount, group.speed);
					int steps = groupLedCount * 2;
					int count = animation_step_count(duration, steps);
					if (count > 0) {
						int step = animation_step(duration, steps);
						if (step >= groupLedCount ? count > step - groupLedCount : count > step) {
							if (group.extra == GROUP_EXTRA_RANDOM) {
								group.color1 = CHSV(random8(), 255, 255);
							}
							else if (group.extra == GROUP_EXTRA_ALTERNATING) {
								group.color3 = group.color1;
								group.color1 = group.color2;
								group.color2 = group.color3;
							}
						}
						fill_solid(&channelData[channelId].leds[group.ledIndex], groupLedCount, CRGB::Black);
						for (int i = 0; i < 4; i++) {
							int led = (((step - i) % steps) + steps) % steps;
							if (led >= groupLedCount) {
								led = steps - led - 1;
							}
							channelData[channelId].leds[group.ledIndex + led] = group.color1;
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
						for (int i = 0; i < groupLedCount; i++) {
							channelData[channelId].leds[group.ledIndex + i] = (i + step) % 3 > 0 ? group.color1 : CRGB::Black;
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

						fill_solid(&channelData[channelId].leds[group.ledIndex], groupLedCount, step == 0 ? group.color1 : CRGB::Black);
						updated = true;
					}
					break;
				}
				case GROUP_MODE_Sequential:
				{
					int steps = groupLedCount;
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
							fill_solid(&channelData[channelId].leds[group.ledIndex], step + 1, group.color1);
							fill_solid(&channelData[channelId].leds[group.ledIndex + step + 1], groupLedCount - (step + 1), group.color2);
						}
						else {
							fill_solid(&channelData[channelId].leds[group.ledIndex + groupLedCount - (step + 1)], step + 1, group.color1);
							fill_solid(&channelData[channelId].leds[group.ledIndex], groupLedCount - (step + 1), group.color2);
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
						fill_solid(&channelData[channelId].leds[group.ledIndex], groupLedCount, CHSV(step, 255, 255));
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
				nscale8_video(&channelData[channelId].leds[group.ledIndex], groupLedCount, channel.brightness);
				}
			}
			break;
		}
		case CHANNEL_MODE_SOFTWARE_PLAYBACK:
		{
			if (trigger_update) {
				auto& data = channelData[channelId];
				for (int i = 0; i < data.ledCount; i++) {
					data.leds[i] = CRGB(data.valuesBuffer[0][i], data.valuesBuffer[1][i], data.valuesBuffer[2][i]);
				}
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
		if (updated) {
			anyUpdate = true;
			if (channelData[channelId].onUpdateCallback != nullptr) {
				channelData[channelId].onUpdateCallback();
			}
		}
	}
	trigger_update = false;
	return anyUpdate;
}

size_t FastLEDController::getEEPROMSize()
{
	return sizeof(channels);
}

void FastLEDController::onUpdateHook(uint8_t channel, void (*callback)(void))
{
	channelData[channel].onUpdateCallback = callback;
}

bool FastLEDController::load() {
	if (useEEPROM) {
		EEPROM.get(EEPROM_ADDRESS, channels);
		for (LEDChannel& channel : channels) {
			if (!isValidLEDChannel(channel)) {
				channel = LEDChannel();
			}
		}
		return true;
	}
	return false;
}

bool FastLEDController::save() {
	if (useEEPROM) {
#ifdef DEBUG
		Serial.println(F("Save to EEPROM."));
#endif
		EEPROM.put(EEPROM_ADDRESS, channels);
		return true;
	}
	return false;
}

void FastLEDController::triggerLEDUpdate()
{
	trigger_update = true;
}

void FastLEDController::setLEDExternalTemperature(uint8_t channel, uint16_t temp)
{
	channelData[channel].temp = temp;
}

void FastLEDController::setLEDColorValues(uint8_t channel, uint8_t color, uint8_t offset, const uint8_t* values, size_t len)
{
	int copyLength = min((int)channelData[channel].ledCount - offset, (int)len);
	if (copyLength > 0) {
		memcpy(channelData[channel].valuesBuffer[color] + offset, values, copyLength);
	}
}

void FastLEDController::clearLEDColorValues(uint8_t channel)
{
	memset(channelData[channel].valuesBuffer[0], 0, sizeof(channelData[channel].ledCount));
}
