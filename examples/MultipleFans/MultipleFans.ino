/*
   Copyright 2020 Leon Kiefer

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
#include <CorsairLightingProtocol.h>
#include <FastLED.h>

#define NUMBER_OF_LEDS_PER_FAN 8

#define DATA_PIN_FAN_1 2
#define DATA_PIN_FAN_2 3
#define DATA_PIN_FAN_3 4
#define DATA_PIN_FAN_4 5
#define DATA_PIN_FAN_5 6
#define DATA_PIN_FAN_6 7
#define DATA_PIN_CHANNEL_2 8

CRGB ledsChannel1[96];
CRGB ledsChannel2[96];

CorsairLightingFirmware firmware = corsairLightingNodePROFirmware();
FastLEDController ledController(true);
CorsairLightingProtocolController cLP(&ledController, &firmware);
CorsairLightingProtocolHID cHID(&cLP);

void setup() {
	// 6 fans on channel 1
	//                          FAN_PIN           CRGB array            offset             number of leds per fan
	FastLED.addLeds<WS2812B, DATA_PIN_FAN_1, GRB>(ledsChannel1, NUMBER_OF_LEDS_PER_FAN * 0, NUMBER_OF_LEDS_PER_FAN);
	FastLED.addLeds<WS2812B, DATA_PIN_FAN_2, GRB>(ledsChannel1, NUMBER_OF_LEDS_PER_FAN * 1, NUMBER_OF_LEDS_PER_FAN);
	FastLED.addLeds<WS2812B, DATA_PIN_FAN_3, GRB>(ledsChannel1, NUMBER_OF_LEDS_PER_FAN * 2, NUMBER_OF_LEDS_PER_FAN);
	FastLED.addLeds<WS2812B, DATA_PIN_FAN_4, GRB>(ledsChannel1, NUMBER_OF_LEDS_PER_FAN * 3, NUMBER_OF_LEDS_PER_FAN);
	FastLED.addLeds<WS2812B, DATA_PIN_FAN_5, GRB>(ledsChannel1, NUMBER_OF_LEDS_PER_FAN * 4, NUMBER_OF_LEDS_PER_FAN);
	FastLED.addLeds<WS2812B, DATA_PIN_FAN_6, GRB>(ledsChannel1, NUMBER_OF_LEDS_PER_FAN * 5, NUMBER_OF_LEDS_PER_FAN);

	// normal strip on channel 2
	FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_2, GRB>(ledsChannel2, 96);
	ledController.addLEDs(0, ledsChannel1, 96);
	ledController.addLEDs(1, ledsChannel2, 96);
}

void loop() {
	cHID.update();

	if (ledController.updateLEDs()) {
		FastLED.show();
	}
}
