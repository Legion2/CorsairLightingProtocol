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

#include <math.h>

#define MAX_TEMP 150
// readings this close to the ends of the ADC range mean open or shorted input
#define ADC_MARGIN 8

CLP::ThermistorTemperatureController::ThermistorTemperatureController()
	: ThermistorTemperatureController(ThermistorConfig()) {}

CLP::ThermistorTemperatureController::ThermistorTemperatureController(const ThermistorConfig& config, uint16_t adcMax)
	: config(config), adcMax(adcMax) {
	for (uint8_t i = 0; i < TEMPERATURE_NUM; i++) {
		sensorPins[i] = PIN_UNSET;
	}
}

void CLP::ThermistorTemperatureController::addSensor(uint8_t index, uint8_t pin) {
	if (index >= TEMPERATURE_NUM) {
		return;
	}
	pinMode(pin, INPUT);
	sensorPins[index] = pin;
}

bool CLP::ThermistorTemperatureController::readSensor(uint8_t temperatureSensor, uint16_t& raw) {
	if (temperatureSensor >= TEMPERATURE_NUM || sensorPins[temperatureSensor] == PIN_UNSET) {
		return false;
	}
	raw = analogRead(sensorPins[temperatureSensor]);
	return raw > ADC_MARGIN && raw < adcMax - ADC_MARGIN;
}

uint16_t CLP::ThermistorTemperatureController::getTemperatureValue(uint8_t temperatureSensor) {
	uint16_t raw;
	if (!readSensor(temperatureSensor, raw)) {
		return 0;
	}
	// voltage divider: series resistor to ground, thermistor to the ADC reference
	const float resistance = config.seriesResistance * (float)(adcMax - raw) / (float)raw;
	// simplified Steinhart-Hart (B parameter) equation
	float temp = logf(resistance / config.nominalResistance) / config.beta;
	temp += 1.0f / (config.nominalTemperature + 273.15f);
	temp = 1.0f / temp - 273.15f;
	return constrain(temp, 0, MAX_TEMP) * 100;
}

bool CLP::ThermistorTemperatureController::isTemperatureSensorConnected(uint8_t temperatureSensor) {
	uint16_t raw;
	return readSensor(temperatureSensor, raw);
}

uint16_t CLP::ThermistorTemperatureController::getVoltageRail12V() { return 0; }

uint16_t CLP::ThermistorTemperatureController::getVoltageRail5V() { return 0; }

uint16_t CLP::ThermistorTemperatureController::getVoltageRail3V3() { return 0; }
