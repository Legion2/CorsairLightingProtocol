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

#define DATA_PIN_CHANNEL_1 2

CRGB ledsChannel1[204];

CorsairLightingFirmwareStorageEEPROM firmwareStorage;
CorsairLightingFirmware firmware(CORSAIR_LIGHTING_NODE_CORE, &firmwareStorage);
FastLEDControllerStorageEEPROM storage;
FastLEDController ledController(&storage);
CorsairLightingProtocolController cLP(&ledController, &firmware);
CorsairLightingProtocolHID cHID(&cLP);

void setup() {
	FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_1, GRB>(ledsChannel1, 204);
	ledController.addLEDs(0, ledsChannel1, 204);
}

void loop() {
	cHID.update();

	if (ledController.updateLEDs()) {
		FastLED.show();
	}
}
