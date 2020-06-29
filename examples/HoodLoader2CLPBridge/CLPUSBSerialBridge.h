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

#include "Arduino.h"

#if (COMMAND_SIZE >= RESPONSE_SIZE)
#define RAWHID_AND_SERIAL_BUFFER_SIZE COMMAND_SIZE
#endif

// Workaround for 16 byte responses don't work on 16U2 see https://github.com/Legion2/CorsairLightingProtocol/pull/152
#define RESPONSE_SIZE_16U2 64

#define SERIAL_SYNCHRONIZATION_TIMEOUT 20
#define SERIAL_RESPONSE_TIMEOUT 10
#define SERIAL_BAUD 1000000

#define RESET_PIN IO_MCU_RESET_PIN

class CLPUSBSerialBridge {
public:
	/**
	 * Create a new CLPUSBSerialBridge with the default Serial Number.
	 */
	CLPUSBSerialBridge(){};
	/**
	 * Create a new CLPUSBSerialBridge and set a Serial Number.
	 *
	 * @param serialNumber the Serial Number of the USB device
	 */
	CLPUSBSerialBridge(const char* serialNumber);
	/**
	 * Setup the bridge connections. Must be called in the Arduino setup function.
	 */
	virtual void begin();
	/**
	 * Reads data from USB and sents it via Serial to the main MCU. Wait for the response and set it back to the USB
	 * host. MUST be called in the Arduino loop function.
	 */
	virtual void handleHID();

private:
	byte rawHIDAndSerialBuffer[RAWHID_AND_SERIAL_BUFFER_SIZE];

	void sendError();
	void sendResponse();
};
