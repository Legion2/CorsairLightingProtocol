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

#include "Arduino.h"
#include "TemperatureController.h"

namespace CLP {

/**
 * The electrical parameters of an NTC thermistor and its voltage divider. The defaults match the common 10k NTC with
 * a 10k series resistor. Water temperature sensors for PCs are usually 10k NTCs as well, but check the beta value in
 * the datasheet (many use 3435 instead of 3950).
 */
struct ThermistorConfig {
	/**
	 * @param seriesResistance the resistance of the fixed resistor between the analog pin and ground in Ohm
	 * @param nominalResistance the resistance of the thermistor at nominalTemperature in Ohm
	 * @param nominalTemperature the temperature in degree Celsius at which the thermistor has nominalResistance
	 * @param beta the beta coefficient of the thermistor from the datasheet
	 */
	ThermistorConfig(float seriesResistance = 10000, float nominalResistance = 10000, float nominalTemperature = 25,
					 float beta = 3950)
		: seriesResistance(seriesResistance),
		  nominalResistance(nominalResistance),
		  nominalTemperature(nominalTemperature),
		  beta(beta) {}
	float seriesResistance;
	float nominalResistance;
	float nominalTemperature;
	float beta;
};

/**
 * A TemperatureController which reads NTC thermistors on analog pins. It does not implement the voltage rail
 * measurements, they always report 0 mV.
 *
 * Thermistor schematic:
 * <pre>
 *     | ---- [10k - Resistor] ---- | ---- [Thermistor] ---- |
 *     |                            |                        |
 *  [Ground]                    Analog Pin              [ADC reference, +5V on AVR]
 * </pre>
 *
 * A sensor is reported as connected when its pin was configured and the reading is neither at the lower nor at the
 * upper end of the ADC range: an unplugged thermistor reads (almost) 0, a shorted one (almost) the maximum. iCUE reads
 * the sensor mask only once when the device is connected, so plug the sensors in before the USB cable.
 */
class ThermistorTemperatureController : public TemperatureController {
public:
	/**
	 * Create a controller for 10k/3950 thermistors on a 10 bit ADC.
	 */
	ThermistorTemperatureController();
	/**
	 * Create a controller for thermistors with custom parameters. All sensors of a controller share the parameters.
	 *
	 * @param config the electrical parameters of the thermistors and the series resistors
	 * @param adcMax the maximum value of analogRead(), 1023 for the 10 bit ADC of AVR boards. Pass 4095 for boards
	 * with a 12 bit ADC or call analogReadResolution(10) on them.
	 */
	ThermistorTemperatureController(const ThermistorConfig& config, uint16_t adcMax = 1023);
	/**
	 * Add a thermistor on an analog pin connected as shown in {@link ThermistorTemperatureController}.
	 *
	 * @param index the index of the sensor
	 * @param pin the Arduino analog pin
	 */
	void addSensor(uint8_t index, uint8_t pin);

protected:
	virtual uint16_t getTemperatureValue(uint8_t temperatureSensor) override;
	virtual bool isTemperatureSensorConnected(uint8_t temperatureSensor) override;
	virtual uint16_t getVoltageRail12V() override;
	virtual uint16_t getVoltageRail5V() override;
	virtual uint16_t getVoltageRail3V3() override;

	static const uint8_t PIN_UNSET = 0xFF;
	const ThermistorConfig config;
	const uint16_t adcMax;
	uint8_t sensorPins[TEMPERATURE_NUM];

	/**
	 * Read the ADC of a sensor and decide whether a thermistor is present.
	 *
	 * @param temperatureSensor the index of the sensor
	 * @param raw receives the ADC reading
	 * @return true if the reading is in the plausible range of a connected thermistor
	 */
	bool readSensor(uint8_t temperatureSensor, uint16_t& raw);
};

}  // namespace CLP
