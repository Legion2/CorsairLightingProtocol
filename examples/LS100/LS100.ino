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
#include <FastLED.h>

// Hint: The channels of the LS100 are swapped in iCUE, so the first channel in iCUE is here channel 2
#define DATA_PIN_CHANNEL_1 2
#define DATA_PIN_CHANNEL_2 3

CRGB ledsChannel1[135];
CRGB ledsChannel2[135];

CorsairLightingFirmware firmware = corsairLS100Firmware();
FastLEDController ledController(true);
CorsairLightingProtocolController cLP(&ledController, &firmware);
CorsairLightingProtocolHID cHID(&cLP);

void setup() {
	FastLED.addLeds<NEOPIXEL, DATA_PIN_CHANNEL_1>(ledsChannel1, 135);
	FastLED.addLeds<NEOPIXEL, DATA_PIN_CHANNEL_2>(ledsChannel2, 135);
	ledController.addLEDs(0, ledsChannel1, 135);
	ledController.addLEDs(1, ledsChannel2, 135);
}

void loop() {
	cHID.update();

	if (ledController.updateLEDs()) {
		FastLED.show();
	}
}