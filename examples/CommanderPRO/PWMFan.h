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

class PWMFan {
public:
	/**
	 * PWM fan which maps speed to power using linear interpolation. This fan does not read the real RPM values. The
	 * Arduino timer for the given pin will be set to higher speed.
	 *
	 * @param pwmPin the Arduino pwm pin for this fan. Not all PWM pins are supported.
	 * @param minRPM the speed in RPM at 0% power
	 * @param maxRPM the speed in RPM at 100% power
	 */
	PWMFan(uint8_t pwmPin, uint16_t minRPM, uint16_t maxRPM);
	virtual void setPower(uint8_t percentage);
	virtual uint8_t calculatePowerFromSpeed(uint16_t rpm);
	virtual uint16_t calculateSpeedFromPower(uint8_t power);

protected:
	const uint8_t pwmPin;
	const uint16_t minRPM;
	const uint16_t maxRPM;
};
