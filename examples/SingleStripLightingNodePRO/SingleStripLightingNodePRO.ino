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

// The number of LEDs per channel, there are two channels.
// 60 LEDs per channel is the maximum iCUE can handle.
#define CHANNEL_LED_COUNT 50

// Total count of LEDs on all channels, the value is calculated based on the leds per channel.
#define NUM_LEDS (CHANNEL_LED_COUNT * 2)

// The Arduino pin where the physical LEDs are connected.
// You can use two pins, one for each channel.
// In this example we use only one pin where both channel are connected in series.
#define DATA_PIN 2

// Should the EEPROM of the Arduino be used to store persistent information like the Hardware Lighting.
// If enabled, the Hardware Lighting configured in iCUE works without a USB connection and even after a restart of the Arduino.
#define USE_EEPROM true

// This will disable the RX and TX built in leds on Arduino Leonardo, Micro and Pro Micro.
#define DISABLE_BUILTIN_LED

LEDController<CHANNEL_LED_COUNT> ledController(USE_EEPROM);
CorsairLightingProtocol cLP(&ledController);

// This array conatins all RGB values for all LEDs of the both channels.
CRGB leds[NUM_LEDS];

// This variable stores the current command which was received.
Command command;

void setup() {
#ifdef DEBUG
	Serial.begin(115200);
#else
#ifdef DISABLE_BUILTIN_LED
#ifdef LED_BUILTIN_RX
	pinMode(LED_BUILTIN_RX, INPUT);
#endif
#ifdef LED_BUILTIN_TX
	pinMode(LED_BUILTIN_TX, INPUT);
#endif
#endif
#endif
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
	ledController.addLeds(0, leds);
	ledController.addLeds(1, &(leds[CHANNEL_LED_COUNT]));
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
