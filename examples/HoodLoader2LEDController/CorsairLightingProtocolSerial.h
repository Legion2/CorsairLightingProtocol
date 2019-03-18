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
#include <ILEDController.h>

#define SERIAL_BUFFER_TIMEOUT 100
#define SERIAL_BAUD 115200

class CorsairLightingProtocolSerial : public CorsairLightingProtocolResponse {
public:
	CorsairLightingProtocolSerial(ILEDController* a, const uint8_t* firmwareVersion);
	void begin();
	bool available() const;
	// should be called in SerialEvent()
	void handleSerial();
	void getCommand(Command& command);
	void handleCommand(const Command& command);
	void sendX(const uint8_t* data, const size_t x) const override;
private:
	byte rawCommand[COMMAND_SIZE];
	uint8_t part = 0;
	unsigned long last_rx = 0;
	ILEDController* const ledController;
	CorsairLightingFirmware corsairLightingFirmware;
};

#endif
