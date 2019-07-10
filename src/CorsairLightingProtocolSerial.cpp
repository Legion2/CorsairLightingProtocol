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

CorsairLightingProtocolSerial::CorsairLightingProtocolSerial(CorsairLightingProtocol* cLP) : cLP(cLP)
{
	Serial.begin(SERIAL_BAUD);
	Serial.setTimeout(SERIAL_TIMEOUT);
}

void CorsairLightingProtocolSerial::update()
{
	bool available = handleSerial();
	if (available)
	{
		Command command;
		memcpy(command.raw, rawCommand, sizeof(command.raw));
		cLP->handleCommand(command, this);
	}
}

bool CorsairLightingProtocolSerial::handleSerial()
{
	if (Serial.available()) {
		delay(SERIAL_TIMEOUT);
		while (Serial.available()) {
			Serial.read();
		}
	}

	Serial.write(42);
	Serial.flush();
	delayMicroseconds(100);

	if (Serial.available()) {
		size_t read = Serial.readBytes(rawCommand, sizeof(rawCommand));
		if (read == sizeof(rawCommand)) {
			return true;
		}
		else {
			byte data[] = { PROTOCOL_RESPONSE_ERROR, (byte)read };
			sendX(data, sizeof(data));
		}
	}
	return false;
}

void CorsairLightingProtocolSerial::sendX(const uint8_t* data, const size_t x) const
{
	Serial.write(data, x);
}
