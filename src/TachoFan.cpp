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
#include "TachoFan.h"

// the Adafruit nRF52 core does not define it; there every pin has an interrupt anyway
#ifndef NOT_AN_INTERRUPT
#define NOT_AN_INTERRUPT -1
#endif

// no pulse for this long means the fan stands still or is unplugged
#define TACHO_TIMEOUT 1500

#define TACHO_SLOTS 6

namespace {
CLP::TachoFan* tachoSlots[TACHO_SLOTS] = {nullptr};

template <uint8_t SLOT>
void tachoInterrupt() {
	tachoSlots[SLOT]->pulse();
}

void (*tachoInterruptForSlot(uint8_t slot))() {
	switch (slot) {
		case 0:
			return tachoInterrupt<0>;
		case 1:
			return tachoInterrupt<1>;
		case 2:
			return tachoInterrupt<2>;
		case 3:
			return tachoInterrupt<3>;
		case 4:
			return tachoInterrupt<4>;
		default:
			return tachoInterrupt<5>;
	}
}
}  // namespace

CLP::TachoFan::TachoFan(uint8_t pwmPin, uint8_t tachoPin, uint16_t minRPM, uint16_t maxRPM)
	: PWMFan(pwmPin, minRPM, maxRPM), tachoPin(tachoPin) {}

CLP::TachoFan::TachoFan(PWMOutput* output, uint8_t tachoPin, uint16_t minRPM, uint16_t maxRPM)
	: PWMFan(output, minRPM, maxRPM), tachoPin(tachoPin) {}

void CLP::TachoFan::begin() {
	PWMFan::begin();
	pinMode(tachoPin, INPUT_PULLUP);
	lastUpdate = (uint16_t)millis();
	if (interruptAttached) {
		return;
	}
	const int interrupt = digitalPinToInterrupt(tachoPin);
	if (interrupt == NOT_AN_INTERRUPT) {
		return;
	}
	for (uint8_t slot = 0; slot < TACHO_SLOTS; slot++) {
		if (tachoSlots[slot] == nullptr) {
			tachoSlots[slot] = this;
			attachInterrupt(interrupt, tachoInterruptForSlot(slot), FALLING);
			interruptAttached = true;
			return;
		}
	}
}

void CLP::TachoFan::pulse() {
	pulseCount++;
	lastPulse = (uint16_t)millis();
}

void CLP::TachoFan::update() {
	// 16 bit timestamps are enough: the update interval and the timeout are far below 65 seconds
	const uint16_t now = (uint16_t)millis();
	noInterrupts();
	const uint16_t pulses = pulseCount;
	pulseCount = 0;
	const uint16_t last = lastPulse;
	interrupts();

	const uint16_t elapsed = now - lastUpdate;
	lastUpdate = now;
	if (pulses > 0) {
		pulseSeen = true;
	}
	if (elapsed == 0 || (uint16_t)(now - last) > TACHO_TIMEOUT) {
		speed = 0;
		return;
	}
	uint32_t rpm = (uint32_t)pulses * 60000UL / (pulsesPerRevolution * elapsed);
	if (rpm > 0xFFFF) {
		rpm = 0xFFFF;
	}
	// smooth the 60 RPM steps of a 500 ms window
	speed = speed == 0 ? rpm : (speed + rpm) / 2;
}

uint16_t CLP::TachoFan::getSpeed() { return speed; }

bool CLP::TachoFan::isConnected() { return pulseSeen; }

void CLP::TachoFan::setPulsesPerRevolution(uint8_t pulses) {
	if (pulses > 0) {
		pulsesPerRevolution = pulses;
	}
}

bool CLP::TachoFan::hasInterrupt() const { return interruptAttached; }
