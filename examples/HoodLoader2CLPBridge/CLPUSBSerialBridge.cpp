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
#include <RawHID.h>
#include <CorsairLightingProtocolConstants.h>

void resetIOMCU() {
	digitalWrite(RESET_PIN, LOW);
	delay(10);
	digitalWrite(RESET_PIN, HIGH);
#ifdef DEBUG
		Serial.println(F("R"));
#endif // DEBUG
}

void CLPUSBSerialBridge::begin()
{
	Serial1.begin(SERIAL_BAUD);
	RawHID.begin(rawHIDAndSerialBuffer, sizeof(rawHIDAndSerialBuffer));
}

bool waitForSynchronization() {
	while (Serial1.available()) {
		Serial1.read();
	}
	Serial1.setTimeout(SERIAL_SYNCHRONIZATION_TIMEOUT);
	byte value;
	size_t read = Serial1.readBytes(&value, 1);
	return read == 1 && value == 42;
}

void CLPUSBSerialBridge::sendError() {
	memset(rawHIDAndSerialBuffer, 0, sizeof(rawHIDAndSerialBuffer));
	rawHIDAndSerialBuffer[0] = PROTOCOL_RESPONSE_ERROR;
	sendResponse();
}

void CLPUSBSerialBridge::sendResponse() {
	RawHID.write(rawHIDAndSerialBuffer, sizeof(rawHIDAndSerialBuffer));
	// free the shared buffer to receive new data
	RawHID.enable();
}

void CLPUSBSerialBridge::handleHID()
{
	if (RawHID.available()) {
		if (!waitForSynchronization()) {
			sendError();
			return;
		}

		Serial1.write(rawHIDAndSerialBuffer, sizeof(rawHIDAndSerialBuffer));
		Serial1.setTimeout(SERIAL_RESPONSE_TIMEOUT);
		size_t read = Serial1.readBytes(rawHIDAndSerialBuffer, sizeof(rawHIDAndSerialBuffer));
		if (read != sizeof(rawHIDAndSerialBuffer)) {
#ifdef DEBUG
			Serial.print(F("T"));
			Serial.println(read);
			Serial.println(rawHIDAndSerialBuffer[0], HEX);
#endif // DEBUG
			sendError();
			return;
		}
		sendResponse();
	}
}
