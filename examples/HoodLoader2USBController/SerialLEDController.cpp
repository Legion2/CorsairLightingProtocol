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
#include <CorsairLightingProtocol.h>
#include "SerialLEDController.h"

void SerialLEDController::handleLEDControl(const Command& command, const CorsairLightingProtocol& callbackclp)
{
	this->clp = &callbackclp;
	Serial1.write(command.raw, sizeof(command.raw));
}

void SerialLEDController::handleCallback()
{
	int readAvailable = Serial.available();
	if (readAvailable > 0) {
		int read = min(sizeof(buff) - part, readAvailable);
		Serial1.readBytes(buff + part, read);
		part += read;
		if (part == sizeof(buff)) {
			if (buff[0] == PROTOCOL_RESPONSE_ERROR) {
				this->clp->sendError();
			}
			else {
				this->clp->send(&buff[1], sizeof(buff) - 1);
			}
			part = 0;
		}
	}
}
