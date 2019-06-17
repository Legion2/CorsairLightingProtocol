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
#ifndef _TemperatureController_h
#define _TemperatureController_h

#include "Arduino.h"

#include "CorsairLightingProtocol.h"
#include "ITemperatureController.h"

#define TEMPERATURE_NUM 4

#define TEMPERATURE_MASK_CONNECTED 0x01
#define TEMPERATURE_MASK_NOT_CONNECTED 0x00

#define VOLTAGE_RAIL_12V 0
#define VOLTAGE_RAIL_5V 1
#define VOLTAGE_RAIL_3V3 2

class TemperatureController : public ITemperatureController {
public:
	virtual void handleTemperatureControl(const Command & command, const CorsairLightingProtocolResponse* response) override;
	virtual uint16_t getTemperature(uint8_t temperatureSensor);
protected:
	// The temperature in hundredths of a degree Celsius.
	virtual uint16_t getTemperatureValue(uint8_t temperatureSensor) = 0;
	virtual bool isTemperatureSensorConnected(uint8_t temperatureSensor) = 0;
	// The voltage in mV.
	virtual uint16_t getVoltageRail12V() = 0;
	// The voltage in mV.
	virtual uint16_t getVoltageRail5V() = 0;
	// The voltage in mV.
	virtual uint16_t getVoltageRail3V3() = 0;
};

#endif
