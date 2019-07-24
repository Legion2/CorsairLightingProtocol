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
#include "CorsairLightingProtocolHID.h"
#include "RawHID.h"

#if defined(USBCON)

#if (RAWHID_TX_SIZE != RESPONSE_SIZE)
#error "USB endpoint must be the same size as the protocol response"
#endif

#if defined(DEBUG) && defined(VERBOSE)
bool printCommand = PRINT_COMMAND;
bool printResponse = PRINT_RESPONSE;
#endif

CorsairLightingProtocolHID::CorsairLightingProtocolHID(CorsairLightingProtocol* cLP) : cLP(cLP)
{
	RawHID.begin(rawhidData, sizeof(rawhidData));
}

void CorsairLightingProtocolHID::update()
{
	if (available())
	{
		Command command;
		getCommand(command);
		cLP->handleCommand(command, this);
	}
}

bool CorsairLightingProtocolHID::available() const
{
	return RawHID.available() > 0;
}

void CorsairLightingProtocolHID::getCommand(Command& command)
{
	auto bytesAvailable = RawHID.available();
	if (bytesAvailable)
	{
		RawHID.readBytes(command.raw, sizeof(command.raw));
#if defined(DEBUG) && defined(VERBOSE)
		if (printCommand) {
			Serial.print(F("Received Command: "));
			Serial.println(command.command, HEX);
		}
#endif
	}
}

void CorsairLightingProtocolHID::sendX(const uint8_t* data, const size_t x) const
{
#if defined(DEBUG) && defined(VERBOSE)
	if (printResponse) {
		Serial.print(F("Send Response: "));
		Serial.println(data[0], HEX);
	}
#endif
	RawHID.write(data, x);
}

#endif
