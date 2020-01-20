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

#include <CorsairLightingProtocolConstants.h>
#include <CorsairLightingProtocolController.h>
#include <CorsairLightingProtocolResponse.h>

// The maximum time in milliseconds needed to receive 64-byte data
#define SERIAL_TIMEOUT 2
#define SERIAL_BAUD 1000000

/**
 * The Serial adapter for CorsairLightingProtocolController. This adapter uses a Serial bridge to access
 * the USB interface of another Arduino board. This is usefull for boards that don't have the USB
 * functionality build in.
 */
class CorsairLightingProtocolSerial : CorsairLightingProtocolResponse {
public:
	/**
	 * Create a new adapter for the controller.
	 *
	 * @param controller the CorsairLightingProtocolController
	 */
	CorsairLightingProtocolSerial(CorsairLightingProtocolController* controller);
	/**
	 * Setup the Serial connection.
	 * This function must be called in setup.
	 */
	void setup();
	/**
	 * Read commands from Serial connection and pass them to the controller
	 * This function must be called in loop.
	 */
	void update();

private:
	byte rawCommand[COMMAND_SIZE];
	CorsairLightingProtocolController* const controller;

	bool handleSerial();
	void sendX(const uint8_t* data, const size_t x) const override;
};
