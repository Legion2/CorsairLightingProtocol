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

void CLPUSBSerialBridge::begin()
{
	pinMode(IO_MCU_RESET_PIN, OUTPUT);
	digitalWrite(IO_MCU_RESET_PIN, LOW);
	Serial1.begin(SERIAL_BAUD);
	RawHID.begin(rawHIDBuffer, sizeof(rawHIDBuffer));
	digitalWrite(IO_MCU_RESET_PIN, HIGH);
}

void CLPUSBSerialBridge::handleHID()
{
	auto bytesAvailable = RawHID.available();
#ifdef DEBUG
	if (bytesAvailable) {
		Serial.print(F("RawHID: "));
		Serial.println(rawHIDBuffer[0]);
		/*Serial.println(F("RawHID:"));
		for (uint8_t i = 0; i < sizeof(rawHIDBuffer); i++) {
			Serial.print(rawHIDBuffer[i], HEX);
			Serial.print(" ");
		}
		Serial.println();*/
	}

#endif // DEBUG
	while (bytesAvailable)
	{
		byte buffer[RAWHID_TO_SERIAL_BUFFER_SIZE];
		size_t read = min(sizeof(buffer), bytesAvailable);
		RawHID.readBytes(buffer, read);
		Serial1.write(buffer, read);
		bytesAvailable = RawHID.available();
	}
}

void CLPUSBSerialBridge::handleResponse()
{
	if (pos == sizeof(tx_buffer)) {
#ifdef DEBUG
		Serial.print(F("Callback: "));
		Serial.println(tx_buffer[0]);
		/*Serial.println(F("Callback:"));
		for (uint8_t i = 0; i < sizeof(tx_buffer); i++) {
			Serial.print(tx_buffer[i], HEX);
			Serial.print(" ");
		}
		Serial.println();*/
#endif // DEBUG
		RawHID.write(tx_buffer, sizeof(tx_buffer));
		pos = 0;
	}
}

void CLPUSBSerialBridge::handleSerial()
{
	auto readAvailable = Serial1.available();
	while (readAvailable)
	{
		const unsigned long time = millis();
		if (time - last_rx > SERIAL_BUFFER_TIMEOUT) {
#ifdef DEBUG
			if (pos > 0) {
				Serial.print(F("Timeout"));
			}
#endif // DEBUG
			pos = 0;
		}
		last_rx = time;
		size_t read = min(sizeof(tx_buffer) - pos, readAvailable);
#ifdef DEBUG
		if (read < readAvailable) {
			Serial.print(F("Overflow"));
		}
#endif // DEBUG
		Serial1.readBytes(&tx_buffer[pos], read);
		pos += read;
		readAvailable = Serial1.available();
	}
}
