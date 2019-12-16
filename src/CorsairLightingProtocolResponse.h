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

class CorsairLightingProtocolResponse {
public:
	/**
	 * Send 64 bytes via the CorsairLightingProtocol. All unset bytes will be filled with zeros.
	 *
	 * @param data the array with the data
	 * @param size the length of the array
	 */
	virtual void send(const uint8_t* data, size_t size) const;
	/**
	 * Send an error.
	 */
	virtual void sendError() const;
	/**
	 * Send data from program memory.
	 *
	 * @param data the array with the data, the pointer must point to program memory
	 * @param size the length of the array which should be send.
	 */
	virtual void send_P(const uint8_t* data, size_t size) const;
	/**
	 * Send some bytes data via the CorsairLightingProtocol.
	 *
	 * @param data the array with the data
	 * @param x the length of the array which should be send.
	 */
	virtual void sendX(const uint8_t* data, const size_t x) const = 0;
};
