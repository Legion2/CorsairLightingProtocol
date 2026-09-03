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

#define DATA_PIN_CHANNEL_1 2
#define DATA_PIN_CHANNEL_2 3

// Thermistors, see CLP::ThermistorTemperatureController for the wiring
#define TEMP_SENSOR_PIN_1 A0
#define TEMP_SENSOR_PIN_2 A1

// PWM pins with 25 kHz on the ATmega32U4: 5, 9, 10 (pin 6 runs at 31 kHz)
#define FAN_UPDATE_RATE 500
#define PWM_FAN_PIN_1 5
#define PWM_FAN_PIN_2 6
#define PWM_FAN_PIN_3 9
#define PWM_FAN_PIN_4 10

// Memory hint for the ATmega32U4 (2560 bytes of RAM): every LED costs 6 bytes and the sketch needs about 500 bytes of
// free RAM for the stack on top of the "Global variables" reported by the compiler. Two channels with 96 LEDs each
// leave too little for a Commander PRO, use 60 LEDs per channel or only one channel with 96 LEDs.
#define CHANNEL_LED_COUNT 60

CorsairLightingFirmwareStorageEEPROM firmwareStorage;
CorsairLightingFirmware firmware(CORSAIR_COMMANDER_PRO, &firmwareStorage);
CLP::ThermistorTemperatureController temperatureController;
FastLEDControllerStorageEEPROM storage;
FastLEDController ledController(&storage);
CLP::FanControllerStorageEEPROM fanStorage;
CLP::SimpleFanController fanController(&temperatureController, FAN_UPDATE_RATE, &fanStorage);
CorsairLightingProtocolController cLP(&ledController, &temperatureController, &fanController, &firmware);
CorsairLightingProtocolHID cHID(&cLP);

CRGB ledsChannel1[CHANNEL_LED_COUNT];
CRGB ledsChannel2[CHANNEL_LED_COUNT];

// PWM pin, speed at 0% power, speed at 100% power (see the datasheet of your fans)
CLP::PWMFan fan1(PWM_FAN_PIN_1, 0, 2000);
CLP::PWMFan fan2(PWM_FAN_PIN_2, 0, 2000);
CLP::PWMFan fan3(PWM_FAN_PIN_3, 0, 2000);
CLP::PWMFan fan4(PWM_FAN_PIN_4, 0, 2000);

void setup() {
	CLP::disableBuildInLEDs();
	FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_1, GRB>(ledsChannel1, CHANNEL_LED_COUNT);
	FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_2, GRB>(ledsChannel2, CHANNEL_LED_COUNT);
	ledController.addLEDs(0, ledsChannel1, CHANNEL_LED_COUNT);
	ledController.addLEDs(1, ledsChannel2, CHANNEL_LED_COUNT);
	temperatureController.addSensor(0, TEMP_SENSOR_PIN_1);
	temperatureController.addSensor(1, TEMP_SENSOR_PIN_2);
	fanController.addFan(0, &fan1);
	fanController.addFan(1, &fan2);
	fanController.addFan(2, &fan3);
	fanController.addFan(3, &fan4);
	fanController.begin();
}

void loop() {
	cHID.update();

	if (ledController.updateLEDs()) {
		FastLED.show();
	}
	fanController.updateFans();
}
