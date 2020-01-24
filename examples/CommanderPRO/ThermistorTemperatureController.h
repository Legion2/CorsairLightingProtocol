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

#include "TemperatureController.h"
/**
 * This TemperatureController uses Thermistors and Resistors to messure the temperature. It does not implement the
 * voltage rail measurements.
 *
 * Thermistor Schematic:
 * <pre>
 *     | ---- [10k - Resistor] ---- | ---- [Thermistor] ---- |
 *     |                            |                        |
 *  [Ground]                    Analog Pin                 [+5v]
 * </pre>
 */
class ThermistorTemperatureController : public TemperatureController {
public:
	/**
	 * Add a Sensor to the TemperatureController using an Arduino analog pin connected as shown in {@link
	 * ThermistorTemperatureController}.
	 *
	 * @param index the index of the sensorPins
	 * @param pin the Arduino analog pin
	 */
	void addSensor(uint8_t index, uint8_t pin);

protected:
	virtual uint16_t getTemperatureValue(uint8_t temperatureSensor) override;
	virtual bool isTemperatureSensorConnected(uint8_t temperatureSensor) override;
	virtual uint16_t getVoltageRail12V() override;
	virtual uint16_t getVoltageRail5V() override;
	virtual uint16_t getVoltageRail3V3() override;

	uint8_t sensorPins[TEMPERATURE_NUM] = {0};
};
