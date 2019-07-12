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
#ifndef _CorsairLightingProtocolSerial_h
#define _CorsairLightingProtocolSerial_h

#include <CorsairLightingProtocol.h>
#include <CorsairLightingProtocolConstants.h>
#include <CorsairLightingProtocolResponse.h>

//The maximum time in milliseconds needed to receive 64-byte data
#define SERIAL_TIMEOUT 2
#define SERIAL_BAUD 1000000

class CorsairLightingProtocolSerial : CorsairLightingProtocolResponse {
public:
	CorsairLightingProtocolSerial(CorsairLightingProtocol* cLP);
	void update();
private:
	byte rawCommand[COMMAND_SIZE];
	CorsairLightingProtocol* const cLP;

	bool handleSerial();
	void sendX(const uint8_t* data, const size_t x) const override;
};

#endif
