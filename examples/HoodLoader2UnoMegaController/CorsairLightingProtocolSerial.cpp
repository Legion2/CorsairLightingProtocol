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

void CorsairLightingProtocolSerial::begin()
{
	Serial.begin(SERIAL_BAUD);
	Serial.setTimeout(SERIAL_TIMEOUT);
}

bool CorsairLightingProtocolSerial::available() const
{
	return commandAvailable;
}

void CorsairLightingProtocolSerial::handleSerial()
{
	if (Serial.available()) {
		size_t read = Serial.readBytes(rawCommand, sizeof(rawCommand));
		if (read = sizeof(rawCommand)) {
			commandAvailable = true;
		}
	}
}

void CorsairLightingProtocolSerial::getCommand(Command& command)
{
	if (available()) {
		memcpy(command.raw, rawCommand, sizeof(command.raw));
		commandAvailable = false;
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
