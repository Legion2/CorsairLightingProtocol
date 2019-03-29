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
#include "CLPUSBSerialBridge.h"

CLPUSBSerialBridge usbToSerialBridge;

void setup() {
#ifdef DEBUG
	while (!Serial);
	Serial.begin(1000000);
#endif // DEBUG
	delay(5000);
	usbToSerialBridge.begin();
#ifdef DEBUG
	Serial.println((uint8_t)1);
#endif // DEBUG
}

void loop() {
	usbToSerialBridge.handleSerial();
	usbToSerialBridge.handleHID();
	usbToSerialBridge.handleResponse();
}
