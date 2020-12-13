/*
   Copyright 2020 Leon Kiefer

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
#line 17 "UnitTests2.ino"

#include <AUnit.h>

#include "FastLEDController.h"
#include "FastLEDControllerUtils.h"

using namespace aunit;

class FastLEDControllerTest : public TestOnce {
protected:
	void assertCRGB(const CRGB& actual, const CRGB& expected) {
		assertEqual(actual.r, expected.r);
		assertEqual(actual.g, expected.g);
		assertEqual(actual.b, expected.b);
	}
	void assertCRGBArray(const CRGB* const leds, int from, int to, const CRGB& expected) {
		for (int i = from; i <= to; i++) {
			assertCRGB(leds[i], expected);
		}
	}
};

class TestFastLEDController : public FastLEDController {
public:
	TestFastLEDController() : FastLEDController(false) {
	}

	void setLastUpdate(unsigned long time) {
		lastUpdate = time;
	}

	void setCurrentUpdate(unsigned long time) {
		currentUpdate = time;
	}

	using FastLEDController::animation_step;
	using FastLEDController::animation_step_count;
};

test(getLEDs) {
	CRGB leds[10];
	FastLEDController ledController(false);
	ledController.addLEDs(0, leds, 10);
	assertEqual(ledController.getLEDs(0), leds);
	assertEqual(ledController.getLEDs(1), nullptr);
}

testF(FastLEDControllerTest, simpleScaleUp) {
	CRGB leds[20];
	FastLEDController ledController(false);
	fill_solid(leds, 20, CRGB::Black);
	ledController.addLEDs(0, leds, 20);

	fill_solid(leds, 10, CRGB::White);
	CLP::reverse(&ledController, 0);

	assertCRGBArray(leds, 0, 9, CRGB::Black);
	assertCRGBArray(leds, 10, 19, CRGB::White);
}

test(animation_step) {
	TestFastLEDController ledController;
	ledController.setLastUpdate(155);
	ledController.setCurrentUpdate(165);
	assertEqual(ledController.animation_step(5, 100), 0);
	assertEqual(ledController.animation_step(10, 100), 50);
	assertEqual(ledController.animation_step(20, 100), 25);
	assertEqual(ledController.animation_step(40, 100), 12);
	assertEqual(ledController.animation_step(5000, 100), 3);
}

test(animation_step_long) {
	TestFastLEDController ledController;
	ledController.setLastUpdate(3000000000);
	ledController.setCurrentUpdate(300000005);
	assertEqual(ledController.animation_step(5, 100), 0);
	assertEqual(ledController.animation_step(10, 100), 50);
	assertEqual(ledController.animation_step(20, 100), 25);
	assertEqual(ledController.animation_step(40, 100), 12);
	assertEqual(ledController.animation_step(5000, 100), 0);
}

test(animation_step_count) {
	TestFastLEDController ledController;
	ledController.setLastUpdate(155);
	ledController.setCurrentUpdate(165);
	assertEqual(ledController.animation_step_count(5, 100), 200);
	assertEqual(ledController.animation_step_count(10, 100), 100);
	assertEqual(ledController.animation_step_count(20, 100), 50);
	assertEqual(ledController.animation_step_count(40, 100), 25);
	assertEqual(ledController.animation_step_count(5000, 100), 0);
}

test(animation_step_count_offset) {
	TestFastLEDController ledController;
	ledController.setLastUpdate(195);
	ledController.setCurrentUpdate(205);
	assertEqual(ledController.animation_step_count(10000, 100), 1);
}

test(animation_step_count_long) {
	TestFastLEDController ledController;
	ledController.setLastUpdate(3000000000);
	ledController.setCurrentUpdate(3000000010);
	assertEqual(ledController.animation_step_count(5, 100), 200);
	assertEqual(ledController.animation_step_count(10, 100), 100);
	assertEqual(ledController.animation_step_count(20, 100), 50);
	assertEqual(ledController.animation_step_count(40, 100), 25);
	assertEqual(ledController.animation_step_count(5000, 100), 0);
}

void setup() {
	delay(1000);
	Serial.begin(115200);
	while (!Serial)
		;
}

void loop() {
	TestRunner::run();
}
