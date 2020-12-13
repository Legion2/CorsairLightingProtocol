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

FastLEDController::FastLEDController(bool useEEPROM) : temperatureController(nullptr), useEEPROM(useEEPROM) { load(); }

FastLEDController::FastLEDController(TemperatureController* temperatureController, bool useEEPROM)
	: temperatureController(temperatureController), useEEPROM(useEEPROM) {
	load();
}

FastLEDController::~FastLEDController() {
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

CRGB* FastLEDController::getLEDs(uint8_t channel) {
	if (channel >= CHANNEL_NUM) {
		return nullptr;
	}
	return channelData[channel].leds;
}

uint8_t FastLEDController::getLEDCount(uint8_t channel) {
	if (channel >= CHANNEL_NUM) {
		return 0;
	}
	return channelData[channel].ledCount;
}

int FastLEDController::applySpeed(int duration, const GroupSpeed speed) {
	switch (speed) {
		case GroupSpeed::High:
			return duration / 2;
		case GroupSpeed::Medium:
			return duration;
		case GroupSpeed::Low:
			return duration * 2;
		default:
			return duration;
	}
}

int FastLEDController::animation_step(int duration, int steps) {
	int currentStep = ((currentUpdate % (unsigned int)duration) / ((float)duration)) * steps;
	return currentStep;
}

int FastLEDController::animation_step_count(int duration, int steps) {
	unsigned long lastAnimationNumber = lastUpdate / (unsigned int)duration;
	unsigned long currentAnimationNumber = currentUpdate / (unsigned int)duration;
	int lastStep = ((lastUpdate % (unsigned int)duration) / ((float)duration)) * steps;
	int currentStep = ((currentUpdate % (unsigned int)duration) / ((float)duration)) * steps;

	return currentStep - lastStep + (currentAnimationNumber - lastAnimationNumber) * steps;
}

CRGB FastLEDController::randomColor() { return ColorFromPalette(RainbowColors_p, random8(), 255, NOBLEND); }

bool FastLEDController::renderRainbowWave(ChannelData& channelData, LEDGroup& group, int groupLedCount) {
	int duration = applySpeed(3300, group.speed);
	int count = animation_step_count(duration, 256);
	if (count > 0) {
		int step = animation_step(duration, 256);
		int move = group.direction == GroupDirection::Forward ? -3 : 3;
		for (int i = 0; i < groupLedCount; i++) {
			channelData.leds[group.ledIndex + i] = CHSV(step + (i * move), 255, 255);
		}
		return true;
	}
	return false;
}

bool FastLEDController::renderColorShift(ChannelData& channelData, LEDGroup& group, int groupLedCount) {
	int duration = applySpeed(3000, group.speed);
	int count = animation_step_count(duration, 512);
	if (count > 0) {
		int step = animation_step(duration, 512);
		if (count > step) {
			if (group.extra == GroupExtra::Random) {
				group.color1 = group.color2;
				group.color2 = randomColor();
			} else if (group.extra == GroupExtra::Alternating) {
				group.color3 = group.color1;
				group.color1 = group.color2;
				group.color2 = group.color3;
			}
		}
		uint8_t scale;
		if (step < 128) {
			scale = 0;
		} else if (step < 384) {
			scale = ease8InOutApprox(step - 128);
		} else {
			scale = 255;
		}

		fill_solid(&channelData.leds[group.ledIndex], groupLedCount, group.color1.lerp8(group.color2, scale));
		return true;
	}
	return false;
}

bool FastLEDController::renderColorPulse(ChannelData& channelData, LEDGroup& group, int groupLedCount) {
	int duration = applySpeed(3000, group.speed);
	int count = animation_step_count(duration, 512);
	if (count > 0) {
		int step = animation_step(duration, 512);
		if (count > step) {
			if (group.extra == GroupExtra::Random) {
				group.color1 = randomColor();
			} else if (group.extra == GroupExtra::Alternating) {
				group.color3 = group.color1;
				group.color1 = group.color2;
				group.color2 = group.color3;
			}
		}
		uint8_t scale = ease8InOutApprox((uint8_t)step);
		if (step >= 256) {
			scale = 255 - scale;
		}

		fill_solid(&channelData.leds[group.ledIndex], groupLedCount, group.color1 % scale);
		return true;
	}
	return false;
}

bool FastLEDController::renderColorWave(ChannelData& channelData, LEDGroup& group, int groupLedCount) {
	int duration = applySpeed(3000, group.speed);
	int count = animation_step_count(duration, 10000);
	if (count > 0) {
		int step = animation_step(duration, 10000);
		if (count > step) {
			if (group.extra == GroupExtra::Random) {
				group.color1 = group.color2;
				group.color2 = randomColor();
			} else if (group.extra == GroupExtra::Alternating) {
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
			} else {
				color = flag ? group.color2 : group.color1;
				distanceWave = abs(valley + 0.5 - pos);
			}

			uint8_t scale;
			if (distanceWave > 0.25) {
				scale = 0;
			} else {
				scale = 255 - ease8InOutApprox((distanceWave * 4) * 256);
			}
			channelData.leds[group.ledIndex + i] = (color % scale);
		}
		return true;
	}
	return false;
}

bool FastLEDController::renderStatic(ChannelData& channelData, LEDGroup& group, int groupLedCount) {
	fill_solid(&channelData.leds[group.ledIndex], groupLedCount, group.color1);
	return true;
}

bool FastLEDController::renderTemperature(ChannelData& channelData, LEDGroup& group, int groupLedCount) {
	uint16_t currentTemperature;
	const uint8_t& tempGroup = group.tempGroup;
	if (tempGroup == GROUP_TEMP_GROUP_EXTERNAL) {
		currentTemperature = channelData.temp;
	} else if (tempGroup < TEMPERATURE_NUM && temperatureController != nullptr) {
		currentTemperature = temperatureController->getTemperature(tempGroup);
	} else {
		return false;
	}

	CRGB color;
	if (currentTemperature < group.temp1) {
		color = group.color1;
	} else if (currentTemperature < group.temp2) {
		color = group.color1.lerp16(
			group.color2, ((currentTemperature - group.temp1) / ((float)(group.temp2 - group.temp1))) * 65535);
	} else if (currentTemperature < group.temp3) {
		color = group.color2.lerp16(
			group.color3, ((currentTemperature - group.temp2) / ((float)(group.temp3 - group.temp2))) * 65535);
	} else {
		color = group.color3;
	}

	fill_solid(&channelData.leds[group.ledIndex], groupLedCount, color);
	return true;
}

bool FastLEDController::renderVisor(ChannelData& channelData, LEDGroup& group, int groupLedCount) {
	int duration = applySpeed(120 * groupLedCount, group.speed);
	const int resolution = 64;
	int steps = groupLedCount * 2 * resolution;
	int count = animation_step_count(duration, steps);
	if (count > 0) {
		int step = animation_step(duration, steps);
		if (step >= steps / 2 ? count > step - steps / 2 : count > step) {
			if (group.extra == GroupExtra::Random) {
				group.color1 = randomColor();
				group.color2 = randomColor();
			} else if (group.extra == GroupExtra::Alternating) {
				auto temp = group.color1;
				group.color1 = group.color2;
				group.color2 = temp;
			}
		}
		const float gradientLength = 3.0f;
		float pos = step / (float)resolution;
		float gradientMiddlePosition = (pos >= groupLedCount) ? (groupLedCount * 2) - pos : pos;
		gradientMiddlePosition -= 0.5f;
		const float gradientStartPosition = gradientMiddlePosition - (gradientLength / 2);
		const float gradientEndPosition = gradientMiddlePosition + (gradientLength / 2);
		for (int i = 0; i < groupLedCount; i++) {
			CRGB color = CRGB::Black;
			if (i >= gradientStartPosition - 1 && i < gradientStartPosition) {
				color = group.color1;
				color.nscale8((i - (gradientStartPosition - 1)) * 256.0);
			} else if (i >= gradientStartPosition && i < gradientEndPosition) {
				color = blend(group.color1, group.color2, ((i - gradientStartPosition) / gradientLength) * 256);
			} else if (i >= gradientEndPosition && i < gradientEndPosition + 1) {
				color = group.color2;
				color.nscale8((1 - (i - gradientEndPosition)) * 256.0);
			}
			channelData.leds[group.ledIndex + i] = color;
		}
		return true;
	}
	return false;
}

bool FastLEDController::renderMarquee(ChannelData& channelData, LEDGroup& group, int groupLedCount) {
	int duration = applySpeed(700, group.speed);
	int count = animation_step_count(duration, 3);
	if (count > 0) {
		int step = animation_step(duration, 3);
		for (int i = 0; i < groupLedCount; i++) {
			channelData.leds[group.ledIndex + i] = (i + step) % 3 > 0 ? group.color1 : CRGB::Black;
		}
		return true;
	}
	return false;
}

bool FastLEDController::renderBlink(ChannelData& channelData, LEDGroup& group, int groupLedCount) {
	int duration = applySpeed(3000, group.speed);
	int count = animation_step_count(duration, 2);
	if (count > 0) {
		int step = animation_step(duration, 2);
		if (count > step) {
			if (group.extra == GroupExtra::Random) {
				group.color1 = randomColor();
			} else if (group.extra == GroupExtra::Alternating) {
				group.color3 = group.color1;
				group.color1 = group.color2;
				group.color2 = group.color3;
			}
		}

		fill_solid(&channelData.leds[group.ledIndex], groupLedCount, step == 0 ? group.color1 : CRGB::Black);
		return true;
	}
	return false;
}

bool FastLEDController::renderSequential(ChannelData& channelData, LEDGroup& group, int groupLedCount) {
	int steps = groupLedCount;
	int duration = applySpeed(60 * steps, group.speed);
	int count = animation_step_count(duration, steps);
	if (count > 0) {
		int step = animation_step(duration, steps);
		if (count > step) {
			if (group.extra == GroupExtra::Random) {
				group.color2 = group.color1;
				group.color1 = randomColor();
			}
		}

		if (group.direction == GroupDirection::Forward) {
			fill_solid(&channelData.leds[group.ledIndex + groupLedCount - (step + 1)], step + 1, group.color1);
			fill_solid(&channelData.leds[group.ledIndex], groupLedCount - (step + 1), group.color2);
		} else {
			fill_solid(&channelData.leds[group.ledIndex], step + 1, group.color1);
			fill_solid(&channelData.leds[group.ledIndex + step + 1], groupLedCount - (step + 1), group.color2);
		}
		return true;
	}
	return false;
}

bool FastLEDController::renderRainbow(ChannelData& channelData, LEDGroup& group, int groupLedCount) {
	int duration = applySpeed(3000, group.speed);
	int count = animation_step_count(duration, 256);
	if (count > 0) {
		int step = animation_step(duration, 256);
		fill_solid(&channelData.leds[group.ledIndex], groupLedCount, CHSV(step, 255, 255));
		return true;
	}
	return false;
}

bool FastLEDController::updateLEDs() {
	lastUpdate = currentUpdate;
	currentUpdate = millis();
	if (currentUpdate - lastCommand > LED_CONTROLLER_TIMEOUT) {
		timeoutAction();
	}

	bool anyUpdate = false;

	for (int channelId = 0; channelId < CHANNEL_NUM; channelId++) {
		if (channelData[channelId].leds == nullptr) {
			continue;
		}
		bool updated = false;
		LEDChannel& channel = channels[channelId];

		switch (channel.mode) {
			case ChannelMode::Disabled: {
				break;
			}
			case ChannelMode::HardwarePlayback: {
				for (uint8_t groupIndex = 0; groupIndex < channel.groupsSet; groupIndex++) {
					LEDGroup& group = channel.groups[groupIndex];
					int groupLedCount = min((int)channelData[channelId].ledCount - group.ledIndex, (int)group.ledCount);
					if (groupLedCount <= 0) {
						continue;
					}

					bool groupUpdated = false;

					switch (group.mode) {
						case GROUP_MODE_Rainbow_Wave: {
							groupUpdated = renderRainbowWave(channelData[channelId], group, groupLedCount);
							break;
						}
						case GROUP_MODE_Color_Shift: {
							groupUpdated = renderColorShift(channelData[channelId], group, groupLedCount);
							break;
						}
						case GROUP_MODE_Color_Pulse: {
							groupUpdated = renderColorPulse(channelData[channelId], group, groupLedCount);
							break;
						}
						case GROUP_MODE_Color_Wave: {
							groupUpdated = renderColorWave(channelData[channelId], group, groupLedCount);
							break;
						}
						case GROUP_MODE_Static: {
							groupUpdated = renderStatic(channelData[channelId], group, groupLedCount);
							break;
						}
						case GROUP_MODE_Temperature: {
							groupUpdated = renderTemperature(channelData[channelId], group, groupLedCount);
							break;
						}
						case GROUP_MODE_Visor: {
							groupUpdated = renderVisor(channelData[channelId], group, groupLedCount);
							break;
						}
						case GROUP_MODE_Marquee: {
							groupUpdated = renderMarquee(channelData[channelId], group, groupLedCount);
							break;
						}
						case GROUP_MODE_Blink: {
							groupUpdated = renderBlink(channelData[channelId], group, groupLedCount);
							break;
						}
						case GROUP_MODE_Sequential: {
							groupUpdated = renderSequential(channelData[channelId], group, groupLedCount);
							break;
						}
						case GROUP_MODE_Rainbow: {
							groupUpdated = renderRainbow(channelData[channelId], group, groupLedCount);
							break;
						}
						default: {
#ifdef DEBUG
							Serial.print(F("unkown group mode: "));
							Serial.print(group.mode, HEX);
							Serial.println();
#endif
							break;
						}
					}
					if (groupUpdated) {
						nscale8_video(&channelData[channelId].leds[group.ledIndex], groupLedCount, channel.brightness);
						updated = true;
					}
				}
				break;
			}
			case ChannelMode::SoftwarePlayback: {
				if (trigger_update) {
					auto& data = channelData[channelId];
					for (int i = 0; i < data.ledCount; i++) {
						data.leds[i] = CRGB(data.valuesBuffer[0][i], data.valuesBuffer[1][i], data.valuesBuffer[2][i]);
					}
					updated = true;
				}
				break;
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

size_t FastLEDController::getEEPROMSize() { return sizeof(channels); }

void FastLEDController::onUpdateHook(uint8_t channel, void (*callback)(void)) {
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

void FastLEDController::triggerLEDUpdate() { trigger_update = true; }

void FastLEDController::setLEDExternalTemperature(uint8_t channel, uint16_t temp) { channelData[channel].temp = temp; }

void FastLEDController::setLEDColorValues(uint8_t channel, uint8_t color, uint8_t offset, const uint8_t* values,
										  size_t len) {
	int copyLength = min((int)channelData[channel].ledCount - offset, (int)len);
	if (copyLength > 0) {
		memcpy(channelData[channel].valuesBuffer[color] + offset, values, copyLength);
	}
}

void FastLEDController::clearLEDColorValues(uint8_t channel) {
	for (uint8_t*& buffer : channelData[channel].valuesBuffer) {
		memset(buffer, 0, channelData[channel].ledCount);
	}
}

uint8_t FastLEDController::getLEDAutodetectionResult(uint8_t channel) { return channelData[channel].ledCount; }

void FastLEDController::timeoutAction() {
	for (int channelId = 0; channelId < CHANNEL_NUM; channelId++) {
		triggerSave |= setLEDMode(channelId, ChannelMode::HardwarePlayback);
	}
	saveIfNeeded();
}
