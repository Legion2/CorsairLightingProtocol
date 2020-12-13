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

#include "TemperatureController.h"

void LEDController::handleLEDControl(const Command& command, const CorsairLightingProtocolResponse* response) {
	lastCommand = millis();
	auto& data = command.data;
	if (command.command == WRITE_LED_TRIGGER) {
		triggerLEDUpdate();
		saveIfNeeded();
	} else {
		if (data[0] >= CHANNEL_NUM) {
			response->sendError();
			return;
		}
		const uint8_t& channel = data[0];
		switch (command.command) {
			case READ_LED_STRIP_MASK: {
				uint8_t ledMask[GROUPS_NUM];
				for (uint8_t i = 0; i < GROUPS_NUM; i++) {
					if (i < channels[channel].groupsSet) {
						ledMask[i] = getLEDStripMask(channel, i);
					} else {
						ledMask[i] = 0x00;
					}
				}
				response->send(ledMask, sizeof(ledMask));
				// don't send default response
				return;
				break;
			}
			case WRITE_LED_RGB_VALUE: {
#ifdef DEBUG
				Serial.println(F("WriteLedRgbValue"));
#endif
				// TODO
				response->sendError();
				return;
				break;
			}
			case WRITE_LED_COLOR_VALUES: {
				const uint8_t offset = data[1];
				const size_t inputLength = min((size_t)data[2], sizeof(data) - 4);
				const uint8_t color = data[3];
				if (color >= 3) {
					response->sendError();
					return;
				}
				setLEDColorValues(channel, color, offset, data + 4, inputLength);
				break;
			}
			case WRITE_LED_CLEAR: {
				clearLEDColorValues(channel);
				break;
			}
			case WRITE_LED_GROUP_SET: {
				if (channels[channel].groupsSet >= GROUPS_NUM) {
					response->sendError();
					return;
				}
				LEDGroup group;
				group.ledIndex = data[1];
				group.ledCount = data[2];
				group.mode = data[3];
				group.speed = static_cast<GroupSpeed>(data[4]);
				group.direction = static_cast<GroupDirection>(data[5]);
				group.extra = static_cast<GroupExtra>(data[6]);
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
				group.temp1 = CLP::fromBigEndian(data[17], data[18]);
				group.temp2 = CLP::fromBigEndian(data[19], data[20]);
				group.temp3 = CLP::fromBigEndian(data[21], data[22]);

				if (!isValidLEDGroup(group)) {
					response->sendError();
					return;
				}

				triggerSave |= setLEDGroup(channel, channels[channel].groupsSet++, group);
				break;
			}
			case WRITE_LED_EXTERNAL_TEMP: {
				setLEDExternalTemperature(channel, CLP::fromBigEndian(data[2], data[3]));
				break;
			}
			case WRITE_LED_GROUPS_CLEAR: {
				triggerSave |= clearLEDGroups(channel);
				break;
			}
			case WRITE_LED_MODE: {
				const ChannelMode mode = static_cast<ChannelMode>(data[1]);
				if (!isValidChannelMode(mode)) {
#ifdef DEBUG
					Serial.print(F("unkown LED channel mode: "));
					Serial.print(data[1], HEX);
					Serial.println();
#endif
					response->sendError();
					return;
				}

				triggerSave |= setLEDMode(channel, mode);
				break;
			}
			case WRITE_LED_BRIGHTNESS: {
				uint8_t brightness = CLP::convert100To255(data[1]);
				triggerSave |= setLEDBrightness(channel, brightness);
				break;
			}
			case WRITE_LED_COUNT: {
#ifdef DEBUG
				Serial.print(F("WRITE_LED_COUNT: "));
				Serial.print(data[1], HEX);
				Serial.println();
#endif
				// TODO
				channels[channel].ledCount = data[1];
				break;
			}
			case WRITE_LED_PORT_TYPE: {
				const PortType portType = static_cast<PortType>(data[1]);
				if (!isValidPortType(portType)) {
					response->sendError();
					return;
				}
				triggerSave |= setLEDPortType(channel, portType);
				break;
			}
			case WRITE_LED_START_AUTODETECTION: {
				startLEDAutodetection(channel);
				break;
			}
			case READ_LED_AUTODETECTION_RESULTS: {
				const uint8_t result = getLEDAutodetectionResult(channel);
				uint8_t buffer[] = {result};
				response->send(buffer, sizeof(buffer));
				// don't send default response
				return;
				break;
			}
			default: {
#ifdef DEBUG
				Serial.print(F("unkown command: "));
				Serial.print(command.command, HEX);
				Serial.println();
#endif
				response->sendError();
				return;
			}
		}
	}
	response->send(nullptr, 0);
}

bool LEDController::isValidLEDChannel(const LEDChannel& ledChannel) {
	if (isValidChannelMode(ledChannel.mode) && isValidPortType(ledChannel.ledPortType) &&
		ledChannel.groupsSet < GROUPS_NUM) {
		for (uint8_t i = 0; i < ledChannel.groupsSet; i++) {
			if (!isValidLEDGroup(ledChannel.groups[i])) {
				return false;
			}
		}
		return true;
	}
	return false;
}

bool LEDController::isValidLEDGroup(const LEDGroup& ledGroup) {
	return ledGroup.mode <= GROUP_MODE_Rainbow && isValidGroupSpeed(ledGroup.speed) &&
		   isValidGroupDirection(ledGroup.direction) && isValidGroupExtra(ledGroup.extra) &&
		   (ledGroup.tempGroup == GROUP_TEMP_GROUP_EXTERNAL || ledGroup.tempGroup < TEMPERATURE_NUM);
}

const LEDChannel& LEDController::getChannel(uint8_t channelIndex) { return channels[channelIndex]; }

void LEDController::reset() {
	for (auto& channel : channels) {
		channel = LEDChannel();
	}
	save();
}

uint8_t LEDController::getLEDStripMask(uint8_t channel, uint8_t groupIndex) {
	return channels[channel].groups[groupIndex].ledCount;
}

bool LEDController::setLEDGroup(uint8_t channel, uint8_t groupIndex, LEDGroup& group) {
	channels[channel].groups[groupIndex] = group;
	return true;
}

bool LEDController::clearLEDGroups(uint8_t channel) {
	if (channels[channel].groupsSet != 0) {
		channels[channel].groupsSet = 0;
		return true;
	}
	return false;
}

bool LEDController::setLEDMode(uint8_t channel, ChannelMode mode) {
	if (channels[channel].mode != mode) {
		channels[channel].mode = mode;
		return true;
	}
	return false;
}

bool LEDController::setLEDBrightness(uint8_t channel, uint8_t brightness) {
	if (channels[channel].brightness != brightness) {
		channels[channel].brightness = brightness;
		return true;
	}
	return false;
}

bool LEDController::setLEDPortType(uint8_t channel, PortType ledPortType) {
	if (channels[channel].ledPortType != ledPortType) {
		channels[channel].ledPortType = ledPortType;
		return true;
	}
	return false;
}

void LEDController::startLEDAutodetection(uint8_t channel) {
	// Nothing to do here
}

bool LEDController::saveIfNeeded() {
	if (triggerSave) {
		triggerSave = false;
		return save();
	}
	return false;
}
