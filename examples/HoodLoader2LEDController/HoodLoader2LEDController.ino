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
#include <ILEDController.h>
#include <CorsairLightingProtocol.h>
#include <LEDController.h>
#include <FastLED.h>

// iCUE changes to a very slow mode when there are more than 50 leds per channel
#define CHANNEL_LED_COUNT 50
#define NUM_LEDS CHANNEL_LED_COUNT * 2
#define DATA_PIN 2

LEDController<CHANNEL_LED_COUNT> ledController(true);
CorsairLightingProtocol cLP(&ledController);

CRGB leds[NUM_LEDS];

void setup() {
#ifdef DEBUG
	Serial.begin(115200);
#endif
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
	ledController.addLeds(0, leds);
	ledController.addLeds(1, &(leds[CHANNEL_LED_COUNT]));
	cLP.begin();
}

void loop() {
	if (cLP.available())
	{
		Command command;
		cLP.getCommand(command);
		cLP.handleCommand(command);
	}

	if (ledController.updateLEDs()) {
		FastLED.show();
	}
}
