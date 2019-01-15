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
#include "HID-Project.h"

CorsairLightingProtocol_ CorsairLightingProtocol;

CorsairLightingProtocol_::CorsairLightingProtocol_(void)
{
	RawHID.begin(rawhidData, sizeof(rawhidData));
}

void CorsairLightingProtocol_::begin()
{
}

bool CorsairLightingProtocol_::available()
{
	return RawHID.available() > 0;
}

void CorsairLightingProtocol_::getCommand(Command& command)
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

void CorsairLightingProtocol_::handleCommand(const Command& command)
{
	if (command.command >= 0x10 && command.command < 0x30) {
		sendError();
	}
	else if (command.command >= 0x30 && command.command < 0x40) {
		LEDController().handleLEDControl(command);
	}
	else {
		CorsairLightingFirmware().handleFirmwareCommand(command);
	}
}

void CorsairLightingProtocol_::send(const uint8_t* data, size_t size) {
	uint8_t response[RESPONSE_SIZE];
	memset(response, 0x00, sizeof(response));
	if (size + 1 > sizeof(response)) {
		return;
	}
	memcpy(response + 1, data, size);
	RawHID.write(response, sizeof(response));
}

void CorsairLightingProtocol_::sendError() {
	uint8_t response[RESPONSE_SIZE];
	memset(response, 0x00, sizeof(response));
	response[0] = PROTOCOL_RESPONSE_ERROR;
	RawHID.write(response, sizeof(response));
}

void CorsairLightingProtocol_::send_P(const uint8_t* data, size_t size) {
	uint8_t response[RESPONSE_SIZE];
	memset(response, 0x00, sizeof(response));
	if (size + 1 > sizeof(response)) {
		return;
	}
	memcpy_P(response + 1, data, size);
	RawHID.write(response, sizeof(response));
}

