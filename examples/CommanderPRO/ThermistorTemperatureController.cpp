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
#include "ThermistorTemperatureController.h"

#define B_COEFFICIENT 3950
#define TEMPERATURE_REFERENCE (273.15 + 25)
#define RESISTENCE_REFERENCE 10000
#define RESISTENCE_DIVIDER 10000
#define MAX_TEMP 150

void ThermistorTemperatureController::addSensor(uint8_t index, uint8_t pin)
{
	if (index < sizeof(sensorPins)) {
		pinMode(pin, INPUT);
		sensorPins[index] = pin;
	}
}

uint16_t ThermistorTemperatureController::getTemperatureValue(uint8_t temperatureSensor)
{
	if (!isTemperatureSensorConnected(temperatureSensor)) {
		return 0;
	}
	int read = analogRead(sensorPins[temperatureSensor]);

	double resistence = (RESISTENCE_DIVIDER * (double)(1023 - read)) / read;

	double temp = resistence / RESISTENCE_REFERENCE; // (R/Ro)
	temp = log(temp); // ln(R/Ro)
	temp /= B_COEFFICIENT; // * (1/B)
	temp += 1.0 / TEMPERATURE_REFERENCE; // + (1/To)
	temp = 1.0 / temp; // invert
	temp -= 273.15; // convert to °C

	return constrain(temp, 0, MAX_TEMP) * 100;
}

bool ThermistorTemperatureController::isTemperatureSensorConnected(uint8_t temperatureSensor)
{
	return sensorPins[temperatureSensor] != 0;
}

uint16_t ThermistorTemperatureController::getVoltageRail12V()
{
	return 0;
}

uint16_t ThermistorTemperatureController::getVoltageRail5V()
{
	return 0;
}

uint16_t ThermistorTemperatureController::getVoltageRail3V3()
{
	return 0;
}
