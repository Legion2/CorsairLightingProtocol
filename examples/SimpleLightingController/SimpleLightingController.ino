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
#include <LEDController.h>
#include <FastLED.h>

CRGB leds[CHANNEL_LED_COUNT * 2];

void setup() {
#ifdef DEBUG
	Serial.begin(115200);
#endif
	FastLED.addLeds<NEOPIXEL, 2>(leds, CHANNEL_LED_COUNT * 2);
	LEDController.addLeds(0, leds);
	LEDController.addLeds(1, &(leds[CHANNEL_LED_COUNT]));
	CorsairLightingProtocol.begin();
}

void loop() {
	if (CorsairLightingProtocol.available())
	{
		Command command;
		CorsairLightingProtocol.getCommand(command);
		CorsairLightingProtocol.handleCommand(command);
	}

	if (LEDController.updateLEDs()) {
		FastLED.show();
	}
}
