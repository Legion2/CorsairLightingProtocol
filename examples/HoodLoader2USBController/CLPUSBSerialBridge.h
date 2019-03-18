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

#define RAWHID_TO_SERIAL_BUFFER_SIZE 16
#define SERIAL_TO_RAWHID_BUFFER_SIZE 64
#define SERIAL_BUFFER_TIMEOUT 100
#define SERIAL_BAUD 115200

class CLPUSBSerialBridge {
public:
	virtual void begin();
	virtual void handleHID();
	virtual void handleResponse();
	// should be called in SerialEvent()
	virtual void handleSerial();
private:
	byte rawHIDBuffer[64];
	byte tx_buffer[SERIAL_TO_RAWHID_BUFFER_SIZE];
	uint8_t pos = 0;
	unsigned long last_rx = 0;
};

#endif

