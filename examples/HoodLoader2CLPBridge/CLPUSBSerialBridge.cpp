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

#include <CorsairLightingProtocolConstants.h>
#include <RawHID.h>

void resetIOMCU() {
	digitalWrite(RESET_PIN, LOW);
	delay(10);
	digitalWrite(RESET_PIN, HIGH);
#ifdef DEBUG
	Serial.println(F("R"));
#endif  // DEBUG
}

CLPUSBSerialBridge::CLPUSBSerialBridge(const char* serialNumber) {
	CLP::RawHID.setSerialNumber(serialNumber);
}

void CLPUSBSerialBridge::begin() {
	Serial1.begin(SERIAL_BAUD);
	CLP::RawHID.begin(rawHIDAndSerialBuffer, sizeof(rawHIDAndSerialBuffer));
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
	memset(rawHIDAndSerialBuffer, 0, RESPONSE_SIZE_16U2);
	rawHIDAndSerialBuffer[0] = PROTOCOL_RESPONSE_ERROR;
	sendResponse();
}

void CLPUSBSerialBridge::sendResponse() {
#ifdef DEBUG
	Serial.print(F("R"));
	Serial.println(rawHIDAndSerialBuffer[0], HEX);
#endif  // DEBUG
	CLP::RawHID.write(rawHIDAndSerialBuffer, RESPONSE_SIZE_16U2);
	// free the shared buffer to receive new data
	CLP::RawHID.enable();
}

void CLPUSBSerialBridge::handleHID() {
	if (CLP::RawHID.available()) {
#ifdef DEBUG
		Serial.print(F("C"));
		Serial.println(rawHIDAndSerialBuffer[0], HEX);
		unsigned long time = micros();
#endif  // DEBUG
		if (!waitForSynchronization()) {
#ifdef DEBUG
			Serial.println(F("S"));
#endif  // DEBUG
			sendError();
			return;
		}

		Serial1.write(rawHIDAndSerialBuffer, COMMAND_SIZE);
		Serial1.setTimeout(SERIAL_RESPONSE_TIMEOUT);
		size_t read = Serial1.readBytes(rawHIDAndSerialBuffer, RESPONSE_SIZE);
		if (read != RESPONSE_SIZE) {
#ifdef DEBUG
			Serial.print(F("T"));
			Serial.println(read);
			Serial.println(rawHIDAndSerialBuffer[0], HEX);
#endif  // DEBUG
			sendError();
			return;
		}
		sendResponse();

#ifdef DEBUG
		unsigned long duration = micros() - time;
		Serial.print(F("D"));
		Serial.println(duration);
#endif  // DEBUG
	}
}
