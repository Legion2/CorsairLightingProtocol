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
#include "CorsairLightingProtocol.h"
#include "CorsairLightingFirmware.h"
#include "LEDController.h"
#include "RawHID.h"

const uint8_t firmware_version[FIRMWARE_VERSION_SIZE] PROGMEM = { 0x00, 0x07, 0x00 };

CorsairLightingProtocol::CorsairLightingProtocol(ILEDController* aLEDController) : corsairLightingFirmware(firmware_version), ledController(aLEDController), temperatureController(NULL), fanController(NULL) {}

CorsairLightingProtocol::CorsairLightingProtocol(ILEDController* aLEDController, ITemperatureController* temperatureController, IFanController* fanController) : corsairLightingFirmware(firmware_version), ledController(aLEDController), temperatureController(temperatureController), fanController(fanController) {}

void CorsairLightingProtocol::begin()
{
	RawHID.begin(rawhidData, sizeof(rawhidData));
}

bool CorsairLightingProtocol::available() const
{
	return RawHID.available() > 0;
}

void CorsairLightingProtocol::getCommand(Command& command)
{
	auto bytesAvailable = RawHID.available();
	if (bytesAvailable)
	{
		if (bytesAvailable != COMMAND_SIZE) {//TODO why is this always false
#ifdef DEBUG
			Serial.print(F("bytesAvailable: "));
			Serial.println(bytesAvailable);
#endif // DEBUG
			return;
		}
		RawHID.readBytes(command.raw, sizeof(command.raw));
	}
}

void CorsairLightingProtocol::handleCommand(const Command& command)
{
	if (command.command < 0x10) {
		corsairLightingFirmware.handleFirmwareCommand(command, this);
	}
	else if (command.command >= 0x10 && command.command < 0x20) {
		if (temperatureController != NULL) {
			temperatureController->handleTemperatureControl(command, this);
		}
		else {
			sendError();
		}
	}
	else if (command.command >= 0x20 && command.command < 0x30) {
		if (fanController != NULL) {
			fanController->handleFanControl(command, this);
		}
		else {
			sendError();
		}
	}
	else if (command.command >= 0x30 && command.command < 0x40) {
		ledController->handleLEDControl(command, this);
	}
	else {
		sendError();
	}
}

void CorsairLightingProtocol::sendX(const uint8_t* data, const size_t x) const
{
	RawHID.write(data, x);
}

