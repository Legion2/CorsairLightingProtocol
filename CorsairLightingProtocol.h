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

#ifndef _CORSAIRLIGHTINGPROTOCOL_h
#define _CORSAIRLIGHTINGPROTOCOL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#define COMMAND_SIZE USB_EP_SIZE
#define RESPONSE_SIZE USB_EP_SIZE

struct Command {
	union {
		struct {
			uint8_t command;
			uint8_t data[COMMAND_SIZE - 1];
		};
		uint8_t raw[COMMAND_SIZE];
	};
};

class CorsairLightingProtocol_
{
private:
	uint8_t rawhidData[COMMAND_SIZE];
public:
	CorsairLightingProtocol_(void);
	void begin();
	bool available();
	void getCommand(Command& command);
	void handleCommand(const Command& command);
	void response(const uint8_t * data, size_t size, const uint8_t offset);
	void response(const uint8_t data);
	void response_P(const uint8_t * data, size_t size, const uint8_t offset);
};

extern CorsairLightingProtocol_ CorsairLightingProtocol;

#endif
