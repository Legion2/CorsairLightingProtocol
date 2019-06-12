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
#ifndef _CorsairLightingProtocolCallback_h
#define _CorsairLightingProtocolCallback_h

#include "Arduino.h"

class CorsairLightingProtocolResponse {
public:
	virtual void send(const uint8_t* data, size_t size) const;
	virtual void sendError() const;
	virtual void send_P(const uint8_t* data, size_t size) const;
	// Send x byte data via the CorsairLightingProtocol.
	virtual void sendX(const uint8_t* data, const size_t x) const = 0;
};

#endif