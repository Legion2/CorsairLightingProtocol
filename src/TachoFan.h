/*
   Copyright 2026 Thomas Neumann

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
#include "PWMFan.h"

namespace CLP {

/**
 * A 4-pin PWM fan whose tacho signal is read, so iCUE shows the real speed and a stalled or unplugged fan is detected.
 * The tacho wire is an open collector output, the pin is configured with the internal pull-up. Standard fans give two
 * pulses per revolution.
 *
 * Pins with an external interrupt are handled automatically. For other pins call pulse() from your own interrupt
 * routine on every falling edge, see the CommanderPROTacho example for a pin change interrupt on the ATmega32U4.
 *
 * The speed is calculated from the number of pulses per update cycle of the fan controller, which is immune to the
 * interrupt latency caused by FastLED.show(). The update rate of the controller sets the resolution: 500 ms give 60
 * RPM.
 */
class TachoFan : public PWMFan {
public:
	/**
	 * Create a fan with its own PWM output and a tacho input.
	 *
	 * @param pwmPin the Arduino PWM pin for this fan, see PWMOutput for the supported pins
	 * @param tachoPin the pin connected to the tacho wire of the fan
	 * @param minRPM the speed in RPM at 0% power, used to map iCUE speed requests to power
	 * @param maxRPM the speed in RPM at 100% power, used to map iCUE speed requests to power
	 */
	TachoFan(uint8_t pwmPin, uint8_t tachoPin, uint16_t minRPM, uint16_t maxRPM);
	/**
	 * Create a fan on a shared PWM output with its own tacho input.
	 *
	 * @param output the shared output
	 * @param tachoPin the pin connected to the tacho wire of the fan
	 * @param minRPM the speed in RPM at 0% power, used to map iCUE speed requests to power
	 * @param maxRPM the speed in RPM at 100% power, used to map iCUE speed requests to power
	 */
	TachoFan(PWMOutput* output, uint8_t tachoPin, uint16_t minRPM, uint16_t maxRPM);
	virtual void begin() override;
	virtual void update() override;
	/**
	 * @return the measured speed in RPM, 0 if no pulse was seen for more than a second
	 */
	virtual uint16_t getSpeed() override;
	/**
	 * @return true once the tacho signal produced a pulse after begin()
	 */
	virtual bool isConnected() override;
	/**
	 * Count one tacho pulse. Called from the interrupt routine; call it yourself on every falling edge of the tacho
	 * signal if the tacho pin has no external interrupt.
	 */
	void pulse();
	/**
	 * @param pulses the number of tacho pulses per revolution, 2 for standard PC fans
	 */
	void setPulsesPerRevolution(uint8_t pulses);
	/**
	 * @return true if the tacho pin has an external interrupt and pulses are counted automatically
	 */
	bool hasInterrupt() const;

protected:
	const uint8_t tachoPin;
	uint8_t pulsesPerRevolution = 2;
	bool interruptAttached = false;
	volatile uint16_t pulseCount = 0;
	volatile uint16_t lastPulse = 0;
	bool pulseSeen = false;
	uint16_t lastUpdate = 0;
	uint16_t speed = 0;
};

}  // namespace CLP
