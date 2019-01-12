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
#include "CorsairLightingFirmware.h"
#include <FastLED.h>
#include "HID-Project.h"

uint8_t rawhidData[64];

void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);
	RawHID.begin(rawhidData, sizeof(rawhidData));
}

void loop() {
	// Check if there is new data from the RawHID device
	auto bytesAvailable = RawHID.available();
	if (bytesAvailable)
	{
		if (bytesAvailable != 16) {
			Serial.print(F("bytesAvailable: "));
			Serial.print(bytesAvailable);
			Serial.print("\n");
			return;
		}
		int command = RawHID.read();
		uint8_t buffer[15];
		RawHID.readBytes(buffer, sizeof(buffer));
		handleCommand(command, buffer);
	}
}

