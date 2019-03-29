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
	Serial1.setTimeout(SERIAL_TIMEOUT);
	RawHID.begin(rawHIDBuffer, sizeof(rawHIDBuffer));
}

void CLPUSBSerialBridge::handleHID()
{
#ifdef DEBUG
	if (RawHID.available()) {
		Serial.print(F("H"));
		Serial.println(rawHIDBuffer[0], HEX);
	}
#endif // DEBUG
	while (RawHID.available())
	{
		Serial1.write(RawHID.read());
	}
}

void CLPUSBSerialBridge::handleResponse()
{
	if (responseAvailable) {
#ifdef DEBUG
		Serial.print(F("C"));
		Serial.println(tx_buffer[1], HEX);
#endif // DEBUG
		RawHID.write(tx_buffer, sizeof(tx_buffer));
		responseAvailable = false;
	}
}

void CLPUSBSerialBridge::handleSerial()
{
	if (Serial1.available())
	{
		size_t read = Serial1.readBytes(tx_buffer, sizeof(tx_buffer));
		if (read == sizeof(tx_buffer)) {
			responseAvailable = true;
		}
#ifdef DEBUG
		else {
			Serial.print(F("T"));
			Serial.println(read);
		}
#endif // DEBUG
	}
}
