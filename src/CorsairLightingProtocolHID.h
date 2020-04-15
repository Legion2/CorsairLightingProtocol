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
#include "CorsairLightingProtocolConstants.h"
#include "CorsairLightingProtocolController.h"
#include "CorsairLightingProtocolResponse.h"
#include "RawHID.h"

#if defined(SUPPORT_RAW_HID)

#if defined(DEBUG) && defined(VERBOSE)
extern bool printCommand;
extern bool printResponse;
#endif

/**
 * The HID Adapter for CorsairLightingProtocolController. This adapter uses the USB HID interface directly to mimic a
 * corsair device. This adapter can only be used when the USB interface is accessable by the sketch.
 */
class CorsairLightingProtocolHID : CorsairLightingProtocolResponse {
public:
	/**
	 * Create a new adapter for CorsairLightingProtocolController using the default Serial Number.
	 *
	 * @param controller the CorsairLightingProtocolController
	 */
	CorsairLightingProtocolHID(CorsairLightingProtocolController* controller);
	/**
	 * Create a new adapter for using the given Serial Number for the usb interface.
	 *
	 * @param controller the CorsairLightingProtocolController
	 * @param serialNumber the Serial Number used for the USB interface
	 */
	CorsairLightingProtocolHID(CorsairLightingProtocolController* controller, const char* serialNumber);
	/**
	 * Read commands form HID interface and pass them to the contoller. This function must be called in loop.
	 */
	void update();

protected:
	uint8_t rawhidData[COMMAND_SIZE];
	CorsairLightingProtocolController* const controller;

	bool available() const;
	void getCommand(Command& command);
	void sendX(const uint8_t* data, const size_t x) const override;
};

#endif
