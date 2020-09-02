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

// Hint: The channels are swapped in iCUE, so the first channel in iCUE is here channel 2
#define DATA_PIN_CHANNEL_1 2  // For the monitor backlight
#define DATA_PIN_CHANNEL_2 3

CRGB ledsChannel1[84];
CRGB ledsChannel2[105];

CorsairLightingFirmware firmware = corsairLS100Firmware();
FastLEDController ledController(true);
CorsairLightingProtocolController cLP(&ledController, &firmware);
CorsairLightingProtocolHID cHID(&cLP);

void setup() {
	FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_1, GRB>(ledsChannel1, 84);
	FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_2, GRB>(ledsChannel2, 105);
	ledController.addLEDs(0, ledsChannel1, 84);
	ledController.addLEDs(1, ledsChannel2, 105);
	ledController.onUpdateHook(0, []() {
		// increase the brightness of channel 1 when using iCUE, because iCUE only set brightness to max 50%
		CLP::fixIcueBrightness(&ledController, 0);
		// gamma correction with gamma value 2.0. Use napplyGamma_video for other gamma values.
		CLP::gammaCorrection(&ledController, 0);
		// napplyGamma_video(ledsChannel1, 84, 2.2);
	});
}

void loop() {
	cHID.update();

	if (ledController.updateLEDs()) {
		FastLED.show();
	}
}
