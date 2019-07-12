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
#ifndef _CLPUSBSerialBridge_h
#define _CLPUSBSerialBridge_h

#include "Arduino.h"
#include <CorsairLightingProtocolConstants.h>

#if (COMMAND_SIZE == RESPONSE_SIZE)
	#define RAWHID_AND_SERIAL_BUFFER_SIZE COMMAND_SIZE
#endif

#define SERIAL_SYNCHRONIZATION_TIMEOUT 20
#define SERIAL_RESPONSE_TIMEOUT 10
#define SERIAL_BAUD 1000000

#define RESET_PIN IO_MCU_RESET_PIN

class CLPUSBSerialBridge {
public:
	virtual void begin();
	virtual void handleHID();
private:
	byte rawHIDAndSerialBuffer[RAWHID_AND_SERIAL_BUFFER_SIZE];

	void sendError();
	void sendResponse();
};

#endif
