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
#include "LEDController.h"
#ifdef USE_EEPROM
#include <EEPROM.h>
#endif

uint16_t combine(const byte& byte1, const byte& byte2) {
	uint16_t t = byte1;
	t = t << 8;
	t |= byte2;
	return t;
}

LEDController_::LEDController_() {
	load();
}

void LEDController_::addLeds(uint8_t channel, CRGB const * led_buffer) {
	volatileData[channel].led_buffer = led_buffer;
}

void LEDController_::handleLEDControl(const Command& command) {
	auto& data = command.data;
	if (command.command == WRITE_LED_TRIGGER) {
		trigger_update = true;
		if (trigger_save) {
			trigger_save = false;
			save();
		}
	}
	else if (data[0] < CHANNEL_NUM) {
		Channel& ledChannel = channels[data[0]];
		switch (command.command)
		{
		case READ_LED_STRIP_MASK:
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
			CorsairLightingProtocol.send(ledMask, sizeof(ledMask));
			return;
			break;
		}
		case WRITE_LED_RGB_VALUE:
		{
#ifdef DEBUG
			Serial.println(F("WriteLedRgbValue"));
#endif
			break;
		}
		case WRITE_LED_COLOR_VALUES:
		{
			const uint8_t offset = data[1];
			const uint8_t length = data[2];
			const uint8_t color = data[3];
			if (color >= 3) {
				return;
			}
			if (offset + length > CHANNEL_LED_COUNT) {
				return;
			}
			memcpy(volatileData[data[0]].values_buffer[color] + offset, data + 4, length);
			break;
		}
		case WRITE_LED_CLEAR:
		{
#ifdef DEBUG
			Serial.print(F("WriteLedClear"));
			Serial.println(data[0], HEX);
#endif
			break;
		}
		case WRITE_LED_GROUP_SET:
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
			trigger_save = true;
			break;
		}
		case WRITE_LED_EXTERNAL_TEMP:
		{
			ledChannel.temp = combine(data[2], data[3]);
			break;
		}
		case WRITE_LED_GROUPS_CLEAR:
		{
#ifdef DEBUG
			Serial.println(F("WriteLedGroupsClear"));
#endif
			if (ledChannel.groupsSet != 0) {
				ledChannel.groupsSet = 0;
				trigger_save = true;
			}
			break;
		}
		case WRITE_LED_MODE:
		{
			if (ledChannel.ledMode != data[1]) {
				ledChannel.ledMode = data[1];
				trigger_save = true;
			}
			break;
		}
		case WRITE_LED_BRIGHTNESS:
		{
			uint8_t brightness = data[1] * 2.5546875f;
			if (ledChannel.brightness != brightness) {
				ledChannel.brightness = brightness;
				trigger_save = true;
			}
			break;
		}
		case WRITE_LED_COUNT:
		{
#ifdef DEBUG
			Serial.print(F("WRITE_LED_COUNT: "));
			Serial.print(data[1], HEX);
			Serial.println();
#endif
			ledChannel.ledCount = data[1];
		}
		case WRITE_LED_PORT_TYPE:
		{
			if (ledChannel.ledPortType != data[1]) {
				ledChannel.ledPortType = data[1];
				trigger_save = true;
			}
			break;
		}
		default:
		{
#ifdef DEBUG
			Serial.print(F("unkown command: "));
			Serial.print(command.command, HEX);
			Serial.print("\n");
#endif
			break;
		}
		}
	}
	CorsairLightingProtocol.send(nullptr, 0);
}

void LEDController_::addColors(CRGB * led_buffer, const CRGB& color, const uint8_t* values, uint8_t length) {
	for (int i = 0; i < length; i++) {
		led_buffer[i] += color % values[i];
	}
}

int LEDController_::applySpeed(int duration, byte speed) {
	switch (speed)
	{
	case GROUP_SPEED_HIGH:
		return duration / 2;
	case GROUP_SPEED_MEDIUM:
		return duration;
	case GROUP_SPEED_LOW:
		return duration * 2;
	}
}

/*
returns the current step of the animation
*/
int LEDController_::animation_step(int duration, int steps) {
	int currentStep = ((currentUpdate % duration) / ((float)duration)) * steps;
	return currentStep;
}
/*
returns the number of steps since the last update
*/
int LEDController_::animation_step_count(int duration, int steps) {
	long lastAnimationNumber = lastUpdate / duration;
	long currentAnimationNumber = currentUpdate / duration;
	int lastStep = ((lastUpdate % duration) / ((float)duration)) * steps;
	int currentStep = ((currentUpdate % duration) / ((float)duration)) * steps;

	return currentStep - lastStep + (currentAnimationNumber - lastAnimationNumber) * steps;
}

bool LEDController_::updateLEDs()
{
	lastUpdate = currentUpdate;
	currentUpdate = millis();

	bool updated = false;

	for (int channelId = 0; channelId < CHANNEL_NUM; channelId++) {
		Channel& channel = channels[channelId];
		switch (channel.ledMode)
		{
		case CHANNEL_MODE_DISABLED:
		{
			break;
		}
		case CHANNEL_MODE_ON:
		{
			for (int i = 0; i < channel.groupsSet; i++) {
				const Group& group = channel.groups[i];
				switch (group.mode)
				{
				case GROUP_MODE_Color_Pulse:
				{
					int duration = applySpeed(3000, group.speed);
					int count = animation_step_count(duration, 512);
					if (count > 0) {
						int step = animation_step(duration, 512);
						if (count > step) {
							if (group.extra & GROUP_EXTRA_RANDOM) {
								group.color1 = CHSV(random8(), 255, 255);
							}
							else if (group.extra & GROUP_EXTRA_ALTERNATING) {
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
				case GROUP_MODE_Static:
				{
					fill_solid(&volatileData[channelId].led_buffer[group.ledIndex], group.ledCount, group.color1 % channel.brightness);
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
							if (group.extra & GROUP_EXTRA_RANDOM) {
								group.color1 = CHSV(random8(), 255, 255);
							}
							else if (group.extra & GROUP_EXTRA_ALTERNATING) {
								group.color3 = group.color1;
								group.color1 = group.color2;
								group.color2 = group.color3;
							}
						}

						if (step >= group.ledCount) {
							step = steps - step - 1;
						}
						fadeToBlackBy(&volatileData[channelId].led_buffer[group.ledIndex], group.ledCount, 100);
						volatileData[channelId].led_buffer[group.ledIndex + step] = group.color1 % channel.brightness;
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
			Serial.print(F("unkown led mode: "));
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

bool LEDController_::load() {
#ifdef USE_EEPROM
#ifdef DEBUG
	Serial.println(F("Save to EEPROM."));
#endif
	EEPROM.put(0, channels);
	return true;
#endif
	return false;
}

bool LEDController_::save() {
#ifdef USE_EEPROM
	EEPROM.get(0, channels);
	return true;
#endif
	return false;
}

LEDController_ LEDController;