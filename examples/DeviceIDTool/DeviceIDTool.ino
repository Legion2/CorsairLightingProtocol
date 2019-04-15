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
//
// UPLOAD THIS TO THE ARDUINO AND OPEN SERIAL MONITOR WITH BOUDRATE 115200
//
#include <EEPROM.h>

#define EEPROM_ADDRESS_DEVICE_ID 0

void printDeviceID(uint8_t* deviceId) {
	char tmp[16];
	for (size_t i = 0; i < 4; i++) {
		sprintf(tmp, "%.2X", deviceId[i]);
		Serial.print(tmp);
		if (i < 3) Serial.print(F(" "));
	}
}

bool isNullID(uint8_t* deviceId) {
	return !(deviceId[0] | deviceId[1] | deviceId[2] | deviceId[3]);
}

void setup() {
	Serial.begin(115200);
	Serial.setTimeout(100);
	uint8_t deviceID[4];
	EEPROM.get(EEPROM_ADDRESS_DEVICE_ID, deviceID);

	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB
	}
	Serial.print(F("Current DeviceID: "));
	printDeviceID(deviceID);
	Serial.println();
	Serial.println(F("Set a new DeviceID by typing it in the Serial Monitor. The new DeviceID must be in same format as above. 4 Blocks with 2 digits separated by white space."));
	Serial.println();

}

void loop() {
	if (Serial.available()) {
		String inputString = Serial.readStringUntil('\n');
		if (inputString.length() < 11) {
			Serial.println(F("Input is too short!"));
			Serial.println(F("Do not forget the leading zeroes!"));
			Serial.println();
		}
		else {
			uint8_t newDeviceID[4];

			newDeviceID[0] = strtol(&inputString[0], NULL, 16);
			newDeviceID[1] = strtol(&inputString[3], NULL, 16);
			newDeviceID[2] = strtol(&inputString[6], NULL, 16);
			newDeviceID[3] = strtol(&inputString[9], NULL, 16);
			Serial.println(F("Set DeviceID to: "));
			printDeviceID(newDeviceID);
			Serial.println();
			if (isNullID(newDeviceID)) {
				Serial.println(F("This is a special DeviceID, it will generate a new random DeviceID next time iCUE controls the device!"));
			}
			Serial.println();
			EEPROM.put(EEPROM_ADDRESS_DEVICE_ID, newDeviceID);
		}
	}
}