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
#include <CorsairLightingProtocol.h>
#include <CorsairLightingProtocolSerial.h>
#include <CorsairLightingNodePRO.h>
#include <FastLEDController.h>
#include <FastLED.h>

#define CHANNEL_LED_COUNT 60

#define DATA_PIN_CHANNEL_1 2
#define DATA_PIN_CHANNEL_2 3

FastLEDController<CHANNEL_LED_COUNT> ledController(true);
CorsairLightingProtocol cLP(&ledController, firmware_version);
CorsairLightingProtocolSerial cLPS(&cLP);

CRGB ledsChannel1[CHANNEL_LED_COUNT];
CRGB ledsChannel2[CHANNEL_LED_COUNT];

void setup() {
	/*
	YOU MUST NOT USE Serial!
	Serial is used by CorsairLightingProtocolSerial!
	*/
	FastLED.addLeds<NEOPIXEL, DATA_PIN_CHANNEL_1>(ledsChannel1, CHANNEL_LED_COUNT);
	FastLED.addLeds<NEOPIXEL, DATA_PIN_CHANNEL_2>(ledsChannel2, CHANNEL_LED_COUNT);
	ledController.addLeds(0, ledsChannel1);
	ledController.addLeds(1, ledsChannel2);
}

void loop() {
	cLPS.update();

	if (ledController.updateLEDs()) {
		FastLED.show();
	}
}
