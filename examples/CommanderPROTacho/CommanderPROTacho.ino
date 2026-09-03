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
/*
   A Commander PRO for a water cooling loop on a SparkFun Pro Micro (ATmega32U4):

   - one LED channel with 96 LEDs, e.g. six Corsair fans with 16 LEDs each on an RGB fan hub
   - one 10k NTC water temperature sensor
   - six radiator fans, three per radiator, each radiator on one PWM output (splitter cable for the PWM wire),
	 every fan with its own tacho wire so iCUE shows the real speed of each fan

   iCUE shows six fan ports. The fans of one radiator share the PWM signal, so the highest speed requested for any of
   the three fans is applied to all of them. Configure the fan curves in iCUE on "Temp #1" of the Commander PRO: the
   curve then runs on the board and keeps working while iCUE is not running (boot, BIOS, sleep).

   Pin plan (Pro Micro):
	 2         LED data
	 9, 10     PWM radiator A and B (exactly 25 kHz)
	 0, 1, 3, 7  tacho fans 1-4 (external interrupts)
	 8, 14     tacho fans 5-6 (pin change interrupts, see below)
	 A0        water temperature sensor: 10k resistor to GND, NTC to VCC, sensor between them on A0
   Pins 0 and 1 are the hardware serial port, so Serial1 is not available for debugging in this setup.
*/
#include <CorsairLightingProtocol.h>
#include <FastLED.h>

#define DATA_PIN_CHANNEL_1 2
#define CHANNEL_LED_COUNT 96

#define TEMP_SENSOR_PIN A0
// Most 10k water temperature sensors for PCs have a beta of 3435, Corsair's own sensors 3950. Check the datasheet and
// compare the reading with a thermometer.
#define TEMP_SENSOR_BETA 3435

#define FAN_UPDATE_RATE 500
#define PWM_PIN_RADIATOR_A 9
#define PWM_PIN_RADIATOR_B 10
// speed of the fans at 0% and 100% PWM, only used to map speed requests from iCUE to PWM
#define FAN_MIN_RPM 300
#define FAN_MAX_RPM 2000

CorsairLightingFirmwareStorageEEPROM firmwareStorage;
CorsairLightingFirmware firmware(CORSAIR_COMMANDER_PRO, &firmwareStorage);
// all sensors of the controller share these thermistor parameters
// series resistor, nominal resistance, nominal temperature, beta
CLP::ThermistorConfig waterSensor(10000, 10000, 25, TEMP_SENSOR_BETA);
CLP::ThermistorTemperatureController temperatureController(waterSensor);
FastLEDControllerStorageEEPROM storage;
FastLEDController ledController(&storage);
CLP::FanControllerStorageEEPROM fanStorage;
CLP::SimpleFanController fanController(&temperatureController, FAN_UPDATE_RATE, &fanStorage);
CorsairLightingProtocolController cLP(&ledController, &temperatureController, &fanController, &firmware);
CorsairLightingProtocolHID cHID(&cLP);

CRGB ledsChannel1[CHANNEL_LED_COUNT];

CLP::PWMOutput radiatorA(PWM_PIN_RADIATOR_A);
CLP::PWMOutput radiatorB(PWM_PIN_RADIATOR_B);
// shared PWM output, tacho pin, min RPM, max RPM
CLP::TachoFan fan1(&radiatorA, 0, FAN_MIN_RPM, FAN_MAX_RPM);
CLP::TachoFan fan2(&radiatorA, 1, FAN_MIN_RPM, FAN_MAX_RPM);
CLP::TachoFan fan3(&radiatorA, 3, FAN_MIN_RPM, FAN_MAX_RPM);
CLP::TachoFan fan4(&radiatorB, 7, FAN_MIN_RPM, FAN_MAX_RPM);
CLP::TachoFan fan5(&radiatorB, 8, FAN_MIN_RPM, FAN_MAX_RPM);
CLP::TachoFan fan6(&radiatorB, 14, FAN_MIN_RPM, FAN_MAX_RPM);

#if defined(__AVR_ATmega32U4__)
// Pins 8 (PB4) and 14 (PB3) have no external interrupt on the ATmega32U4, they are served by the pin change
// interrupt of port B. Every falling edge is one tacho pulse.
ISR(PCINT0_vect) {
	static uint8_t lastPortB = PINB;
	const uint8_t portB = PINB;
	const uint8_t fallingEdges = lastPortB & ~portB;
	lastPortB = portB;
	if (fallingEdges & _BV(PB4)) {
		fan5.pulse();
	}
	if (fallingEdges & _BV(PB3)) {
		fan6.pulse();
	}
}

void enablePinChangeInterrupts() {
	PCMSK0 |= _BV(PCINT4) | _BV(PCINT3);
	PCICR |= _BV(PCIE0);
}
#else
void enablePinChangeInterrupts() {
	// boards with an interrupt on every pin do not need this
}
#endif

void setup() {
	CLP::disableBuildInLEDs();
	FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_1, GRB>(ledsChannel1, CHANNEL_LED_COUNT);
	ledController.addLEDs(0, ledsChannel1, CHANNEL_LED_COUNT);

	temperatureController.addSensor(0, TEMP_SENSOR_PIN);

	fanController.addFan(0, &fan1);
	fanController.addFan(1, &fan2);
	fanController.addFan(2, &fan3);
	fanController.addFan(3, &fan4);
	fanController.addFan(4, &fan5);
	fanController.addFan(5, &fan6);
	fanController.begin();
	enablePinChangeInterrupts();
}

void loop() {
	cHID.update();

	if (ledController.updateLEDs()) {
		FastLED.show();
	}
	fanController.updateFans();
}
