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

#define CHANNEL_LED_COUNT 96

#define DATA_PIN_CHANNEL_1 2
#define DATA_PIN_CHANNEL_2 3

CorsairLightingFirmware firmware = corsairLightingNodePROFirmware();
FastLEDController ledController(true);
CorsairLightingProtocolController cLP(&ledController, &firmware);
CorsairLightingProtocolHID cLPS(&cLP);

CRGB ledsChannel1[CHANNEL_LED_COUNT];
CRGB ledsChannel2[CHANNEL_LED_COUNT];

bool printLoop = PRINT_LOOP;
bool printUpdate = PRINT_UPDATE;

void setup() {
	Serial.begin(115200);
	Serial.setTimeout(100);
	FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_1, GRB>(ledsChannel1, CHANNEL_LED_COUNT);
	FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_2, GRB>(ledsChannel2, CHANNEL_LED_COUNT);
	ledController.addLEDs(0, ledsChannel1, CHANNEL_LED_COUNT);
	ledController.addLEDs(1, ledsChannel2, CHANNEL_LED_COUNT);
}

void loop() {
	if (printLoop) Serial.println(F("loop"));
	cLPS.update();

	if (ledController.updateLEDs()) {
		if (printUpdate) Serial.println(F("updateLEDs"));
		FastLED.show();
		CLP::printFps(5000);
	}

	if (Serial.available()) {
		static String cmd = "";
		cmd = Serial.readStringUntil('\n');
		processCommand(cmd);
	}
}

void processCommand(String& cmd) {
	if (cmd == F("print DeviceID")) {
		byte deviceId[4];
		firmware.getDeviceID(deviceId);
		CLP::printDeviceID(deviceId);
		Serial.println();
	}
#ifdef VERBOSE
	else if (cmd == F("toggle command")) {
		printCommand = !printCommand;
	} else if (cmd == F("toggle response")) {
		printResponse = !printResponse;
	}
#endif  // VERBOSE
	else if (cmd == F("toggle loop")) {
		printLoop = !printLoop;
	} else if (cmd == F("toggle update")) {
		printUpdate = !printUpdate;
	} else {
		Serial.println(F("Unknown command"));
	}
}
