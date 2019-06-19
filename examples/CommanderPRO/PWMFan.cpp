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

PWMFan::PWMFan(uint8_t pwmPin, uint16_t minRPM ,uint16_t maxRPM) : pwmPin(pwmPin), minRPM(minRPM), maxRPM(maxRPM)
{
	pinMode(pwmPin, OUTPUT);
	analogWrite(pwmPin, 0);
	switch (digitalPinToTimer(pwmPin)) {
	case TIMER0B:/* 3 */
#ifdef DEBUG
		Serial.println(F("Pin not supported as PWM fan pin"));
		Serial.println(F("We don't want to mess up arduino time functions"));
#endif // DEBUG
		break;
	case TIMER3A:/* 5 */
		TCCR3B = (TCCR3B & B11111000) | 0x01;
		break;
	case TIMER4D:/* 6 */
		//PLLFRQ = (PLLFRQ & B11001111) | (0x03 << PLLTM0);
		TCCR4B = (TCCR4B & B11110000) | 0x01;
		break;
	case TIMER1A:/* 9 */
		TCCR1B = (TCCR1B & B11111000) | 0x01;
		break;
	case TIMER1B:/* 10 */
		TCCR1B = (TCCR1B & B11111000) | 0x01;
		break;
	default:
#ifdef DEBUG
		Serial.println(F("Pin not supported as PWM fan pin"));
#endif // DEBUG
		break;
	}
}

void PWMFan::setPower(uint8_t percentage)
{
	analogWrite(pwmPin, percentage);
}

uint8_t PWMFan::calculatePowerFromSpeed(uint16_t rpm)
{
	rpm = constrain(rpm, minRPM, maxRPM);
	return ((float)(rpm - minRPM) / (float)(maxRPM - minRPM)) * 255;
}

uint16_t PWMFan::calculateSpeedFromPower(uint8_t power)
{
	return map(power, 0, 255, minRPM, maxRPM);
}
