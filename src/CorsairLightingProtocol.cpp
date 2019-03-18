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
#if defined(USBCON)

CorsairLightingProtocol::CorsairLightingProtocol(ILEDController* const aLEDController) : ledController(aLEDController){}

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
		if (bytesAvailable != COMMAND_SIZE) {
#ifdef DEBUG
			Serial.print(F("bytesAvailable: "));
			Serial.print(bytesAvailable);
			Serial.print("\n");
#endif // DEBUG
			return;
		}
		RawHID.readBytes(command.raw, sizeof(command.raw));
	}
}

void CorsairLightingProtocol::handleCommand(const Command& command)
{
	if (command.command >= 0x10 && command.command < 0x30) {
		sendError();
	}
	else if (command.command >= 0x30 && command.command < 0x40) {
		ledController->handleLEDControl(command, this);
	}
	else {
		CorsairLightingFirmware().handleFirmwareCommand(command, *this);
	}
}

void CorsairLightingProtocol::sendX(const uint8_t* data, const size_t x) const
{
	RawHID.write(data, x);
}

#endif
