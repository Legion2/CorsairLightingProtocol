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
#include <CorsairLightingProtocol.h>
#include <FastLED.h>

#include "SimpleFanController.h"
#include "ThermistorTemperatureController.h"

#define DATA_PIN_CHANNEL_1 2
#define DATA_PIN_CHANNEL_2 3

#define TEMP_SENSOR_PIN_1 A6
#define TEMP_SENSOR_PIN_2 A8

#define FAN_UPDATE_RATE 500
#define PWM_FAN_PIN_1 5
#define PWM_FAN_PIN_2 6
#define PWM_FAN_PIN_3 9
#define PWM_FAN_PIN_4 10

#define CHANNEL_LED_COUNT 96

CorsairLightingFirmware firmware = corsairCommanderPROFirmware();
ThermistorTemperatureController temperatureController;
FastLEDController ledController(&temperatureController, true);
SimpleFanController fanController(&temperatureController, FAN_UPDATE_RATE,
								  EEPROM_ADDRESS + ledController.getEEPROMSize());
CorsairLightingProtocolController cLP(&ledController, &temperatureController, &fanController, &firmware);
CorsairLightingProtocolHID cHID(&cLP);

CRGB ledsChannel1[CHANNEL_LED_COUNT];
CRGB ledsChannel2[CHANNEL_LED_COUNT];

PWMFan fan1(PWM_FAN_PIN_1, 0, 2000);
PWMFan fan2(PWM_FAN_PIN_2, 0, 2000);
PWMFan fan3(PWM_FAN_PIN_3, 0, 2000);
PWMFan fan4(PWM_FAN_PIN_4, 0, 2000);

void analogWrite25k(int pin, int value)
{
    switch (pin) {
        case 9:
            OCR1A = value;
            break;
        case 10:
            OCR1B = value;
            break;
        default:
            // no other pin will work
            break;
    }
}

void setup() {
	CLP::disableBuildInLEDs();
	FastLED.addLeds<NEOPIXEL, DATA_PIN_CHANNEL_1>(ledsChannel1, CHANNEL_LED_COUNT);
	FastLED.addLeds<NEOPIXEL, DATA_PIN_CHANNEL_2>(ledsChannel2, CHANNEL_LED_COUNT);
	ledController.addLEDs(0, ledsChannel1, CHANNEL_LED_COUNT);
	ledController.addLEDs(1, ledsChannel2, CHANNEL_LED_COUNT);
	temperatureController.addSensor(0, TEMP_SENSOR_PIN_1);
	temperatureController.addSensor(1, TEMP_SENSOR_PIN_2);
	fanController.addFan(0, &fan1);
	fanController.addFan(1, &fan2);
	fanController.addFan(2, &fan3);
	fanController.addFan(3, &fan4);
	// Configure Timer 1 for PWM @ 25 kHz.
    TCCR1A = 0;           // undo the configuration done by...
    TCCR1B = 0;           // ...the Arduino core library
    TCNT1  = 0;           // reset timer
    TCCR1A = _BV(COM1A1)  // non-inverted PWM on ch. A
           | _BV(COM1B1)  // same on ch; B
           | _BV(WGM11);  // mode 10: ph. correct PWM, TOP = ICR1
    TCCR1B = _BV(WGM13)   // ditto
           | _BV(CS10);   // prescaler = 1
    ICR1   = 320;         // TOP = 320

    // Set the PWM pins as output.
    pinMode( 9, OUTPUT);
    pinMode(10, OUTPUT);
}

void loop() {
	cHID.update();

	if (ledController.updateLEDs()) {
		FastLED.show();
	}
	fanController.updateFans();
 	analogWrite25k( 9, 110);
 	analogWrite25k(10, 210);
}
