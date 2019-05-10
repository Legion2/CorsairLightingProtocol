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

#define DATA_PIN_CHANNEL_1 2
#define DATA_PIN_CHANNEL_2 3

#define CHANNEL_LED_COUNT 10

LEDController<CHANNEL_LED_COUNT> ledController(true);
CorsairLightingProtocol cLP(&ledController);

CRGB ledsChannel1[CHANNEL_LED_COUNT];
CRGB ledsChannel2[CHANNEL_LED_COUNT];

Command command;

void setup() {
#ifdef LED_BUILTIN_RX
	pinMode(LED_BUILTIN_RX, INPUT);
#endif
#ifdef LED_BUILTIN_TX
	pinMode(LED_BUILTIN_TX, INPUT);
#endif
	FastLED.addLeds<TM1803, DATA_PIN_CHANNEL_1, GBR>(ledsChannel1, CHANNEL_LED_COUNT);
	FastLED.addLeds<TM1803, DATA_PIN_CHANNEL_2, GBR>(ledsChannel2, CHANNEL_LED_COUNT);
	ledController.addLeds(0, ledsChannel1);
	ledController.addLeds(1, ledsChannel2);
	cLP.begin();
}

void loop() {
	if (cLP.available())
	{
		cLP.getCommand(command);
		cLP.handleCommand(command);
	}

	if (ledController.updateLEDs()) {
		FastLED.show();
	}
	else {
		delay(3);
	}
}
