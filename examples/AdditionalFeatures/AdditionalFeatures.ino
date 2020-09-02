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
#define DATA_PIN_CHANNEL_2 3

CRGB ledsChannel1[60];
CRGB ledsChannel2[60];

// Define a custom SerialNumber for the device
const char mySerialNumber[] PROGMEM = "202B6949A967";

CorsairLightingFirmware firmware = corsairLightingNodePROFirmware();
FastLEDController ledController(true);
CorsairLightingProtocolController cLP(&ledController, &firmware);
// Set the SerialNumber here
CorsairLightingProtocolHID cHID(&cLP, mySerialNumber);

void setup() {
	// Disable the build in RX and TX LEDs of the Arduino
	CLP::disableBuildInLEDs();
	// enable reset on DeviceId (FF FF FF FF)
	if (CLP::shouldReset(&firmware)) {
		// reset DeviceId and generate new one
		CLP::reset(&firmware);
		// reset the LEDController Settings
		ledController.reset();
	}
	FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_1, GRB>(ledsChannel1, 60);
	FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_2, GRB>(ledsChannel2, 60);
	ledController.addLEDs(0, ledsChannel1, 60);
	ledController.addLEDs(1, ledsChannel2, 60);
}

void loop() {
	cHID.update();

	if (ledController.updateLEDs()) {
		FastLED.show();
		CLP::printFps(5000);
	}
}
