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
#ifndef _SERIALLEDCONTROLLER_h
#define _SERIALLEDCONTROLLER_h

#include <ILEDController.h>

class SerialLEDController : public ILEDController {
public:
	virtual void handleLEDControl(const Command& command, const CorsairLightingProtocol& clp) override;
	virtual void handleCallback();
private:
	const CorsairLightingProtocol* clp;
	byte buff[RESPONSE_SIZE];
	uint8_t part = 0;
};

#endif

