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

uint16_t combine(const byte& byte1, const byte& byte2) {
	uint16_t t = byte1;
	t = t << 8;
	t |= byte2;
	return t;
}

LEDController_& LEDController()
{
	static LEDController_ obj;
	return obj;
}

void LEDController_::addLeds(uint8_t channel, CRGB const * led_buffer) {
	channels[channel].led_buffer = led_buffer;
}

void LEDController_::handleLEDControl(const Command& command) {
	auto& data = command.data;
	if (data[0] < sizeof(channels)) {
#define ledChannel channels[data[0]]
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
#ifdef DEBUG
			Serial.println(F("WriteLedRgbValue"));
#endif
			break;
		case WRITE_LED_TRIGGER:
#ifdef DEBUG
			Serial.println(F("WRITE_LED_TRIGGER"));
#endif
			break;
		case WRITE_LED_CLEAR:
#ifdef DEBUG
			Serial.println(F("WriteLedClear"));
#endif
			break;
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
			ledChannel.groupsSet = 0;
			break;
		}
		case WRITE_LED_MODE:
		{
#ifdef DEBUG
			Serial.print(F("mode: "));
			Serial.print(data[1], HEX);
			Serial.print("\n");
#endif
			ledChannel.ledMode = data[1];
			break;
		}
		case WRITE_LED_BRIGHTNESS:
		{
			/*Byte 2 is the brightness where
				0x00 = > 0 %
				0x21 = > 33 %
				0x42 = > 66 %
				0x64 = > 100 %
				*/
			ledChannel.brightness = data[1];
			break;
		}
		case WRITE_LED_COUNT:
		{
			ledChannel.ledCount = data[1];
		}
		case WRITE_LED_PORT_TYPE:
		{
#ifdef DEBUG
			Serial.print(F("ledPortType: "));
			Serial.print(data[1], HEX);
			Serial.print("\n");
#endif
			ledChannel.ledPortType = data[1];
			break;
		}
		default:
#ifdef DEBUG
			Serial.print(F("unkown command: "));
			Serial.print(command.command, HEX);
			Serial.print("\n");
#endif
			break;
		}
	}
	CorsairLightingProtocol.send(nullptr, 0);
}

void LEDController_::updateLEDs()
{
}
