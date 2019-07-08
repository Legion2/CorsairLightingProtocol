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

#ifndef _CorsairLightingProtocol_h
#define _CorsairLightingProtocol_h

#include "Arduino.h"
#include "ILEDController.h"
#include "CorsairLightingFirmware.h"
#include "IFanController.h"
#include "ITemperatureController.h"
#include "CorsairLightingProtocolResponse.h"
#include "CorsairLightingProtocolConstants.h"

#if defined(USBCON)

class CorsairLightingProtocol : public CorsairLightingProtocolResponse
{
public:
	CorsairLightingProtocol(ILEDController* l, const uint8_t* firmwareVersion);
	CorsairLightingProtocol(ILEDController* l, ITemperatureController* t, IFanController* f, const uint8_t* firmwareVersion);
	void begin();
	bool available() const;
	void getCommand(Command& command);
	void handleCommand(const Command& command);
	void sendX(const uint8_t * data, const size_t x) const override;
private:
	uint8_t rawhidData[COMMAND_SIZE];
	CorsairLightingFirmware corsairLightingFirmware;
	ILEDController* const ledController;
	ITemperatureController* const temperatureController;
	IFanController* const fanController;
};

#endif
#endif
