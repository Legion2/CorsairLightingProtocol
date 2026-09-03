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
#include "PWMFan.h"

#if defined(__AVR_ATmega32U4__)
// 16 MHz / (2 * 320) = 25 kHz in phase correct PWM mode with ICRn as TOP
#define PWM_TOP 320
#endif

CLP::PWMOutput::PWMOutput(uint8_t pin) : pin(pin) {
	for (uint8_t i = 0; i < MAX_MEMBERS; i++) {
		memberPower[i] = 0;
	}
}

void CLP::PWMOutput::begin() {
	if (configured) {
		return;
	}
	configured = true;
	pinMode(pin, OUTPUT);
#if defined(__AVR_ATmega32U4__)
	switch (digitalPinToTimer(pin)) {
		case TIMER1A: /* 9 */
		case TIMER1B: /* 10 */
			// phase correct PWM (mode 10), TOP = ICR1, no prescaler
			TCCR1A = (TCCR1A & ~(_BV(WGM10))) | _BV(WGM11);
			TCCR1B = (TCCR1B & ~(_BV(WGM12) | _BV(CS12) | _BV(CS11))) | _BV(WGM13) | _BV(CS10);
			ICR1 = PWM_TOP;
			break;
		case TIMER3A: /* 5 */
			TCCR3A = (TCCR3A & ~(_BV(WGM30))) | _BV(WGM31);
			TCCR3B = (TCCR3B & ~(_BV(WGM32) | _BV(CS32) | _BV(CS31))) | _BV(WGM33) | _BV(CS30);
			ICR3 = PWM_TOP;
			break;
		case TIMER4D: /* 6 */
		case TIMER4A: /* 13 */
			// 10 bit high speed timer, keep the 8 bit mode of analogWrite() and use prescaler 2: 31.25 kHz
			TCCR4B = (TCCR4B & B11110000) | 0x02;
			break;
		default:
			// TIMER0B (pin 3) drives millis(), everything else is not a PWM pin
			break;
	}
#elif defined(ARDUINO_ARCH_RP2040)
	analogWriteFreq(25000);
	analogWriteRange(255);
#endif
	write(0);
}

uint8_t CLP::PWMOutput::addMember() {
	if (memberCount >= MAX_MEMBERS) {
		return NO_MEMBER;
	}
	return memberCount++;
}

void CLP::PWMOutput::setMemberPower(uint8_t member, uint8_t power) {
	if (member >= memberCount) {
		return;
	}
	memberPower[member] = power;
	apply();
}

uint8_t CLP::PWMOutput::getPower() const { return appliedPower; }

uint8_t CLP::PWMOutput::getPin() const { return pin; }

void CLP::PWMOutput::apply() {
	uint8_t power = 0;
	for (uint8_t i = 0; i < memberCount; i++) {
		if (memberPower[i] > power) {
			power = memberPower[i];
		}
	}
	if (!configured) {
		begin();
	}
	write(power);
}

void CLP::PWMOutput::write(uint8_t power) {
	appliedPower = power;
#if defined(__AVR_ATmega32U4__)
	const uint16_t compare = (uint32_t)power * PWM_TOP / 255;
	switch (digitalPinToTimer(pin)) {
		case TIMER1A:
			TCCR1A |= _BV(COM1A1);
			OCR1A = compare;
			return;
		case TIMER1B:
			TCCR1A |= _BV(COM1B1);
			OCR1B = compare;
			return;
		case TIMER3A:
			TCCR3A |= _BV(COM3A1);
			OCR3A = compare;
			return;
		default:
			break;
	}
#endif
	analogWrite(pin, power);
}

CLP::PWMFan::PWMFan(uint8_t pwmPin, uint16_t minRPM, uint16_t maxRPM) : PWMFan(new PWMOutput(pwmPin), minRPM, maxRPM) {}

CLP::PWMFan::PWMFan(PWMOutput* output, uint16_t minRPM, uint16_t maxRPM)
	: output(output), member(output->addMember()), minRPM(minRPM), maxRPM(maxRPM) {}

void CLP::PWMFan::begin() { output->begin(); }

void CLP::PWMFan::update() {}

void CLP::PWMFan::setPower(uint8_t power) {
	this->power = power;
	output->setMemberPower(member, power);
}

uint8_t CLP::PWMFan::getPower() const { return power; }

uint16_t CLP::PWMFan::getSpeed() { return calculateSpeedFromPower(power); }

bool CLP::PWMFan::isConnected() { return true; }

uint8_t CLP::PWMFan::calculatePowerFromSpeed(uint16_t rpm) const {
	if (maxRPM <= minRPM) {
		return 255;
	}
	rpm = constrain(rpm, minRPM, maxRPM);
	return ((uint32_t)(rpm - minRPM) * 255) / (maxRPM - minRPM);
}

uint16_t CLP::PWMFan::calculateSpeedFromPower(uint8_t power) const { return map(power, 0, 255, minRPM, maxRPM); }
