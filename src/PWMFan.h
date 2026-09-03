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

namespace CLP {

/**
 * One PWM output pin driving the PWM input of one or more fans. Several fans can share one output (fans of the same
 * radiator on a PWM splitter cable): every fan registers as a member and the output always runs at the highest power
 * requested by any member.
 *
 * On the ATmega32U4 the pins 5, 9 and 10 are driven with exactly 25 kHz as specified for 4-pin fans, pin 6 with
 * 31 kHz. On the RP2040 every pin runs at 25 kHz. Other boards use the default frequency of
 * analogWrite(), which works with most fans but is out of spec. Pin 3 of the ATmega32U4 is not supported because its
 * timer also drives millis().
 */
class PWMOutput {
public:
	/**
	 * @param pin the PWM pin
	 */
	PWMOutput(uint8_t pin);
	/**
	 * Configure the pin and the PWM frequency. Must be called after the Arduino core initialised the timers, so from
	 * setup() and not from a global constructor. setMemberPower() calls it on first use if it was not called before.
	 */
	void begin();
	/**
	 * Register a fan on this output.
	 *
	 * @return the member index to be used with setMemberPower(), or 0xFF if the output is full
	 */
	uint8_t addMember();
	/**
	 * Set the power requested by one member. The output runs at the maximum of all members.
	 *
	 * @param member the member index returned by addMember()
	 * @param power the power in the range 0-255
	 */
	void setMemberPower(uint8_t member, uint8_t power);
	/**
	 * @return the power currently applied to the pin in the range 0-255
	 */
	uint8_t getPower() const;
	/**
	 * @return the pin of this output
	 */
	uint8_t getPin() const;

	static const uint8_t MAX_MEMBERS = 6;
	static const uint8_t NO_MEMBER = 0xFF;

protected:
	const uint8_t pin;
	uint8_t memberCount = 0;
	uint8_t memberPower[MAX_MEMBERS];
	uint8_t appliedPower = 0;
	bool configured = false;

	void apply();
	void write(uint8_t power);
};

/**
 * A 4-pin PWM fan. The speed reported to iCUE is estimated from the power by linear interpolation between minRPM and
 * maxRPM, use TachoFan to report the real speed.
 */
class PWMFan {
public:
	/**
	 * Create a fan with its own PWM output.
	 *
	 * @param pwmPin the Arduino PWM pin for this fan, see PWMOutput for the supported pins
	 * @param minRPM the speed in RPM at 0% power
	 * @param maxRPM the speed in RPM at 100% power
	 */
	PWMFan(uint8_t pwmPin, uint16_t minRPM, uint16_t maxRPM);
	/**
	 * Create a fan on a shared PWM output, for example one of several fans of a radiator on a splitter cable.
	 *
	 * @param output the shared output
	 * @param minRPM the speed in RPM at 0% power
	 * @param maxRPM the speed in RPM at 100% power
	 */
	PWMFan(PWMOutput* output, uint16_t minRPM, uint16_t maxRPM);
	/**
	 * Initialise the output pin. Called by SimpleFanController::begin().
	 */
	virtual void begin();
	/**
	 * Called by the fan controller on every update cycle, before the speed is read.
	 */
	virtual void update();
	/**
	 * Set the power of the fan.
	 *
	 * @param power the power in the range 0-255, 255 means 100%
	 */
	virtual void setPower(uint8_t power);
	/**
	 * @return the last power set for this fan in the range 0-255
	 */
	uint8_t getPower() const;
	/**
	 * @return the speed of the fan in RPM, estimated from the power
	 */
	virtual uint16_t getSpeed();
	/**
	 * @return true if a fan is connected, always true for a fan without tacho signal
	 */
	virtual bool isConnected();
	uint8_t calculatePowerFromSpeed(uint16_t rpm) const;
	uint16_t calculateSpeedFromPower(uint8_t power) const;

protected:
	PWMOutput* const output;
	const uint8_t member;
	const uint16_t minRPM;
	const uint16_t maxRPM;
	uint8_t power = 0;
};

}  // namespace CLP
