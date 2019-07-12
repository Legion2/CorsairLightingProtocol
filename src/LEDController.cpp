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

void LEDController::handleLEDControl(const Command& command, const CorsairLightingProtocolResponse* response)
{
	auto& data = command.data;
	if (command.command == WRITE_LED_TRIGGER) {
		triggerLEDUpdate();
		if (trigger_save) {
			trigger_save = false;
			save();
		}
	}
	else {
		if (data[0] >= CHANNEL_NUM) {
			response->sendError();
			return;
		}
		const uint8_t& channel = data[0];
		switch (command.command)
		{
		case READ_LED_STRIP_MASK:
		{
			uint8_t ledMask[GROUPS_NUM];
			for (uint8_t i = 0; i < GROUPS_NUM; i++) {
				if (i < channels[channel].groupsSet) {
					ledMask[i] = getLEDStripMask(channel, i);
				}
				else {
					ledMask[i] = 0x00;
				}
			}
			response->send(ledMask, sizeof(ledMask));
			// don't send default response
			return;
			break;
		}
		case WRITE_LED_RGB_VALUE:
		{
#ifdef DEBUG
			Serial.println(F("WriteLedRgbValue"));
#endif
			// TODO
			break;
		}
		case WRITE_LED_COLOR_VALUES:
		{
			const uint8_t offset = data[1];
			const uint8_t inputLength = data[2];
			const uint8_t color = data[3];
			if (color >= 3) {
				response->sendError();
				return;
			}
			setLEDColorValues(channel, color, offset, data + 4, inputLength);
			break;
		}
		case WRITE_LED_CLEAR:
		{
			clearLEDColorValues(channel);
			break;
		}
		case WRITE_LED_GROUP_SET:
		{
			if (channels[channel].groupsSet >= GROUPS_NUM) {
				response->sendError();
				return;
			}
			LEDGroup group;
			group.ledIndex = data[1];
			group.ledCount = data[2];
			group.mode = data[3];
			group.speed = data[4];
			group.direction = data[5];
			group.extra = data[6];
			group.tempGroup = data[7];
			group.color1.r = data[8];
			group.color1.g = data[9];
			group.color1.b = data[10];
			group.color2.r = data[11];
			group.color2.g = data[12];
			group.color2.b = data[13];
			group.color3.r = data[14];
			group.color3.g = data[15];
			group.color3.b = data[16];
			group.temp1 = fromBigEndian(data[17], data[18]);
			group.temp2 = fromBigEndian(data[19], data[20]);
			group.temp3 = fromBigEndian(data[21], data[22]);

			trigger_save |= setLEDGroup(channel, channels[channel].groupsSet++, group);
			break;
		}
		case WRITE_LED_EXTERNAL_TEMP:
		{
			setLEDExternalTemperature(channel, fromBigEndian(data[2], data[3]));
			break;
		}
		case WRITE_LED_GROUPS_CLEAR:
		{
			trigger_save |= clearLEDGroups(channel);
			break;
		}
		case WRITE_LED_MODE:
		{
			trigger_save |= setLEDMode(channel, data[1]);
			break;
		}
		case WRITE_LED_BRIGHTNESS:
		{
			uint8_t brightness = convert100To255(data[1]);
			trigger_save |= setLEDBrightness(channel, brightness);
			break;
		}
		case WRITE_LED_COUNT:
		{
#ifdef DEBUG
			Serial.print(F("WRITE_LED_COUNT: "));
			Serial.print(data[1], HEX);
			Serial.println();
#endif
			// TODO
			channels[channel].ledCount = data[1];
			break;
		}
		case WRITE_LED_PORT_TYPE:
		{
			trigger_save |= setLEDPortType(channel, data[1]);
			break;
		}
		default:
		{
#ifdef DEBUG
			Serial.print(F("unkown command: "));
			Serial.print(command.command, HEX);
			Serial.print("\n");
#endif
			response->sendError();
			return;
		}
		}
	}
	response->send(nullptr, 0);
}

uint8_t LEDController::getLEDStripMask(uint8_t channel, uint8_t set)
{
	return channels[channel].groups[set].ledCount;
}

bool LEDController::setLEDGroup(uint8_t channel, uint8_t set, LEDGroup& group)
{
	channels[channel].groups[set] = group;
	return true;
}

bool LEDController::clearLEDGroups(uint8_t channel) {
	if (channels[channel].groupsSet != 0) {
		channels[channel].groupsSet = 0;
		return true;
	}
	return false;
}

bool LEDController::setLEDMode(uint8_t channel, uint8_t mode)
{
	if (channels[channel].ledMode != mode) {
		channels[channel].ledMode = mode;
		return true;
	}
	return false;
}

bool LEDController::setLEDBrightness(uint8_t channel, uint8_t brightness)
{
	if (channels[channel].brightness != brightness) {
		channels[channel].brightness = brightness;
		return true;
	}
	return false;
}

bool LEDController::setLEDPortType(uint8_t channel, uint8_t ledPortType)
{
	if (channels[channel].ledPortType != ledPortType) {
		channels[channel].ledPortType = ledPortType;
		return true;
	}
	return false;
}