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
#include "CorsairLightingProtocolSerial.h"
#include "CorsairLightingFirmware.h"

CorsairLightingProtocolSerial::CorsairLightingProtocolSerial(ILEDController* a, const uint8_t* firmwareVersion) : ledController(a), corsairLightingFirmware(firmwareVersion) {}

bool test = false;

void CorsairLightingProtocolSerial::begin()
{
	Serial.begin(SERIAL_BAUD);
}

bool CorsairLightingProtocolSerial::available() const
{
	return part == sizeof(rawCommand);
}

void CorsairLightingProtocolSerial::handleSerial()
{
	int readAvailable = Serial.available();
	if (readAvailable > 0) {
		digitalWrite(7, !test);
		const unsigned long time = millis();
		if (time - last_rx > SERIAL_BUFFER_TIMEOUT) {
			if (part > 0) {
				test = true;
			}
			part = 0;
		}
		last_rx = time;
		size_t read = min(sizeof(rawCommand) - part, (size_t) readAvailable);
		Serial.readBytes(rawCommand + part, read);
		part += read;
	}
	digitalWrite(7, test);
}

void CorsairLightingProtocolSerial::getCommand(Command& command)
{
	if (available()) {
		memcpy(command.raw, rawCommand, sizeof(command.raw));
		part = 0;
	}
}

void CorsairLightingProtocolSerial::handleCommand(const Command& command)
{
	if (command.command >= 0x10 && command.command < 0x30) {
		sendError();
	}
	else if (command.command >= 0x30 && command.command < 0x40) {
		ledController->handleLEDControl(command, this);
	}
	else {
		corsairLightingFirmware.handleFirmwareCommand(command, this);
	}
}

void CorsairLightingProtocolSerial::sendX(const uint8_t* data, const size_t x) const
{
	Serial.write(data, x);
}
