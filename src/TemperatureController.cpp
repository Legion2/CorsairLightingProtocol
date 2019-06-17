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
#include "TemperatureController.h"
#include "CLPUtils.h"

void TemperatureController::handleTemperatureControl(const Command& command, const CorsairLightingProtocolResponse* response)
{
	switch (command.command)
	{
	case READ_TEMPERATURE_MASK:
	{
		uint8_t mask[TEMPERATURE_NUM];
		for (uint8_t i = 0; i < TEMPERATURE_NUM; i++) {
			mask[i] = isTemperatureSensorConnected(i) ? TEMPERATURE_MASK_CONNECTED : TEMPERATURE_MASK_NOT_CONNECTED;
		}
		response->send(mask, sizeof(mask));
		break;
	}
	case READ_TEMPERATURE_VALUE:
	{
		const uint8_t& tempSensor = command.data[0];
		if (tempSensor >= TEMPERATURE_NUM) {
			response->sendError();
			return;
		}
		uint16_t temp = getTemperatureValue(tempSensor);
		uint8_t tempdata[] = { toBigEndian(temp) };
		response->send(tempdata, sizeof(tempdata));
		break;
	}
	case READ_VOLTAGE_VALUE:
	{
		const uint8_t& rail = command.data[0];
		uint16_t voltage;
		switch (rail)
		{
		case VOLTAGE_RAIL_12V:
		{
			voltage = getVoltageRail12V();
			break;
		}
		case VOLTAGE_RAIL_5V:
		{
			voltage = getVoltageRail5V();
			break;
		}
		case VOLTAGE_RAIL_3V3:
		{
			voltage = getVoltageRail3V3();
			break;
		}
		default:
		{
			response->sendError();
			return;
		}
		}

		uint8_t voltagedata[] = { toBigEndian(voltage) };
		response->send(voltagedata, sizeof(voltagedata));
		break;
	}
	default:
		response->sendError();
	}
}

uint16_t TemperatureController::getTemperature(uint8_t temperatureSensor)
{
	if (temperatureSensor >= TEMPERATURE_NUM) {
		return 0;
	}
	return getTemperatureValue(temperatureSensor);
}
