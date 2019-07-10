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
#ifndef _CORSAIRLIGHTINGPROTOCOLHID_h
#define _CORSAIRLIGHTINGPROTOCOLHID_h

#include "Arduino.h"

#include "CorsairLightingProtocol.h"
#include "CorsairLightingProtocolResponse.h"
#include "CorsairLightingProtocolConstants.h"

#if defined(USBCON)

class CorsairLightingProtocolHID : CorsairLightingProtocolResponse {
public:
	CorsairLightingProtocolHID(CorsairLightingProtocol* cLP);
	void update();
protected:
	uint8_t rawhidData[COMMAND_SIZE];
	CorsairLightingProtocol* const cLP;

	bool available() const;
	void getCommand(Command& command);
	void sendX(const uint8_t* data, const size_t x) const override;
};

#endif
#endif
