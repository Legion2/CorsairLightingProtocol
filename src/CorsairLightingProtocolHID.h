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
#pragma once

#include "Arduino.h"

#include "CorsairLightingProtocolController.h"
#include "CorsairLightingProtocolResponse.h"
#include "CorsairLightingProtocolConstants.h"

#if defined(USBCON)

#if defined(DEBUG) && defined(VERBOSE)
extern bool printCommand;
extern bool printResponse;
#endif

class CorsairLightingProtocolHID : CorsairLightingProtocolResponse {
public:
	CorsairLightingProtocolHID(CorsairLightingProtocolController* cLP);
	CorsairLightingProtocolHID(CorsairLightingProtocolController* cLP, const char* serialNumber);
	void update();
protected:
	uint8_t rawhidData[COMMAND_SIZE];
	CorsairLightingProtocolController* const cLP;

	bool available() const;
	void getCommand(Command& command);
	void sendX(const uint8_t* data, const size_t x) const override;
};

#endif
