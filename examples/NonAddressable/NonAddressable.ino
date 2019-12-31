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

// The Arduino pin where the physical LEDs are connected.
// Must be PWM pins
#define RED_PIN 3
#define GREEN_PIN 5
#define BLUE_PIN 6

CorsairLightingFirmware firmware = corsairLightingNodePROFirmware();
FastLEDController ledController(true);
CorsairLightingProtocolController cLP(&ledController, &firmware);
CorsairLightingProtocolHID cHID(&cLP);

CRGB ledsChannel1[10];

void setup() {
	pinMode(RED_PIN, OUTPUT);
	pinMode(GREEN_PIN, OUTPUT);
	pinMode(BLUE_PIN, OUTPUT);
	ledController.addLEDs(0, ledsChannel1, 10);
	ledController.onUpdateHook(0, []() {
		// use color of first LED of the first channel
		set4PinLEDs(ledsChannel1[0]);
	});
}

void loop() {
	cHID.update();
	ledController.updateLEDs();
}

void set4PinLEDs(const CRGB& color) {
	analogWrite(RED_PIN, color.r);
	analogWrite(GREEN_PIN, color.g);
	analogWrite(BLUE_PIN, color.b);
}
