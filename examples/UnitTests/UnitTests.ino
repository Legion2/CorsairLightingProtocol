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
#line 17 "UnitTests.ino"

#include <AUnit.h>

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
	ledController.addLEDs(0, leds, 10);

	fill_solid(leds, 10, CRGB::White);
	CLP::scale(&ledController, 0, 20);

	assertCRGBArray(leds, 0, 19, CRGB::White);
}

testF(FastLEDControllerTest, simpleScaleDown) {
	CRGB leds[20];
	FastLEDController ledController(false);
	fill_solid(leds, 20, CRGB::Black);
	ledController.addLEDs(0, leds, 20);

	fill_solid(leds, 10, CRGB::White);
	CLP::scale(&ledController, 0, 10);

	assertCRGBArray(leds, 0, 4, CRGB::White);
	assertCRGBArray(leds, 5, 9, CRGB::Black);
}

testF(FastLEDControllerTest, simpleScaleDownBoundaries) {
	CRGB leds[20];
	FastLEDController ledController(false);
	fill_solid(leds, 20, CRGB::Black);
	ledController.addLEDs(0, leds, 20);

	leds[0] = CRGB::White;
	leds[19] = CRGB::Red;
	CLP::scale(&ledController, 0, 5);

	assertCRGBArray(leds, 0, 0, CRGB::White);
	assertCRGBArray(leds, 1, 3, CRGB::Black);
	assertCRGBArray(leds, 4, 4, CRGB::Red);
}

testF(FastLEDControllerTest, simpleScaleIdentity) {
	CRGB leds[20];
	FastLEDController ledController(false);
	fill_solid(leds, 20, CRGB::Black);
	ledController.addLEDs(0, leds, 10);

	fill_solid(leds, 10, CRGB::White);
	CLP::scale(&ledController, 0, 10);

	assertCRGBArray(leds, 0, 9, CRGB::White);
	assertCRGBArray(leds, 10, 19, CRGB::Black);
}

testF(FastLEDControllerTest, scaleLongStrip) {
	CRGB leds[41];
	FastLEDController ledController(false);
	fill_solid(leds, 41, CRGB::Black);
	ledController.addLEDs(0, leds, 27);

	fill_solid(leds, 27, CRGB::White);
	CLP::scale(&ledController, 0, 41);

	assertCRGBArray(leds, 0, 40, CRGB::White);
}

testF(FastLEDControllerTest, LT100) {
	CRGB leds[30];
	FastLEDController ledController(false);
	fill_solid(leds, 30, CRGB::Black);
	ledController.addLEDs(0, leds, 30);

	leds[0] = CRGB::White;
	fill_solid(leds + 1, 26, CRGB::Blue);
	CLP::SegmentScaling segments[2] = {{1, 4}, {26, 26}};
	CLP::scaleSegments(&ledController, 0, segments, 2);

	assertCRGBArray(leds, 0, 3, CRGB::White);
	assertCRGBArray(leds, 4, 29, CRGB::Blue);
}

testF(FastLEDControllerTest, singleSegmentScaleUp) {
	CRGB leds[20];
	FastLEDController ledController(false);
	fill_solid(leds, 20, CRGB::Black);
	ledController.addLEDs(0, leds, 10);

	fill_solid(leds, 10, CRGB::White);
	CLP::SegmentScaling segments[] = {{10, 20}};
	CLP::scaleSegments(&ledController, 0, segments, 1);

	assertCRGBArray(leds, 0, 19, CRGB::White);
}

testF(FastLEDControllerTest, multiScaleUp) {
	CRGB leds[30];
	FastLEDController ledController(false);
	fill_solid(leds, 30, CRGB::Black);
	ledController.addLEDs(0, leds, 10);

	fill_solid(leds + 5, 5, CRGB::White);
	CLP::SegmentScaling segments[] = {{5, 10}, {5, 20}};
	CLP::scaleSegments(&ledController, 0, segments, 2);

	assertCRGBArray(leds, 0, 9, CRGB::Black);
	assertCRGBArray(leds, 10, 29, CRGB::White);
}

testF(FastLEDControllerTest, multiScaleDown) {
	CRGB leds[30];
	FastLEDController ledController(false);
	fill_solid(leds, 30, CRGB::Black);
	ledController.addLEDs(0, leds, 30);

	fill_solid(leds + 10, 20, CRGB::White);
	CLP::SegmentScaling segments[] = {{10, 5}, {20, 5}};
	CLP::scaleSegments(&ledController, 0, segments, 2);

	assertCRGBArray(leds, 0, 4, CRGB::Black);
	assertCRGBArray(leds, 5, 9, CRGB::White);
}

testF(FastLEDControllerTest, singleSegmentScaleDown) {
	CRGB leds[20];
	FastLEDController ledController(false);
	fill_solid(leds, 20, CRGB::Black);
	ledController.addLEDs(0, leds, 20);

	fill_solid(leds, 10, CRGB::White);
	CLP::SegmentScaling segments[] = {{20, 10}};
	CLP::scaleSegments(&ledController, 0, segments, 1);

	assertCRGBArray(leds, 0, 4, CRGB::White);
	assertCRGBArray(leds, 5, 9, CRGB::Black);
}

testF(FastLEDControllerTest, SegmentScaleOverlap) {
	CRGB leds[15];
	FastLEDController ledController(false);
	fill_solid(leds, 15, CRGB::Black);
	ledController.addLEDs(0, leds, 15);

	fill_solid(leds, 5, CRGB::White);
	CLP::SegmentScaling segments[] = {{5, 10}, {10, 5}};
	CLP::scaleSegments(&ledController, 0, segments, 2);

	assertCRGBArray(leds, 0, 9, CRGB::White);
	assertCRGBArray(leds, 10, 14, CRGB::Black);
}

testF(FastLEDControllerTest, SegmentScaleOverlapInverted) {
	CRGB leds[15];
	FastLEDController ledController(false);
	fill_solid(leds, 15, CRGB::Black);
	ledController.addLEDs(0, leds, 15);

	fill_solid(leds, 10, CRGB::White);
	CLP::SegmentScaling segments[] = {{10, 5}, {5, 10}};
	CLP::scaleSegments(&ledController, 0, segments, 2);

	assertCRGBArray(leds, 0, 4, CRGB::White);
	assertCRGBArray(leds, 5, 14, CRGB::Black);
}

testF(FastLEDControllerTest, SegmentScaleMix) {
	CRGB leds[30];
	FastLEDController ledController(false);
	fill_solid(leds, 30, CRGB::Black);
	ledController.addLEDs(0, leds, 30);

	fill_solid(leds, 5, CRGB::White);
	fill_solid(leds + 5, 20, CRGB::Red);
	fill_solid(leds + 25, 5, CRGB::Blue);
	CLP::SegmentScaling segments[] = {{5, 10}, {20, 5}, {5, 10}};
	CLP::scaleSegments(&ledController, 0, segments, 3);

	assertCRGBArray(leds, 0, 9, CRGB::White);
	assertCRGBArray(leds, 10, 14, CRGB::Red);
	assertCRGBArray(leds, 15, 24, CRGB::Blue);
}

testF(FastLEDControllerTest, SegmentScaleMixInverted) {
	CRGB leds[30];
	FastLEDController ledController(false);
	fill_solid(leds, 30, CRGB::Black);
	ledController.addLEDs(0, leds, 25);

	fill_solid(leds, 10, CRGB::White);
	fill_solid(leds + 10, 5, CRGB::Red);
	fill_solid(leds + 15, 10, CRGB::Blue);
	CLP::SegmentScaling segments[] = {{10, 5}, {5, 20}, {10, 5}};
	CLP::scaleSegments(&ledController, 0, segments, 3);

	assertCRGBArray(leds, 0, 4, CRGB::White);
	assertCRGBArray(leds, 5, 24, CRGB::Red);
	assertCRGBArray(leds, 25, 29, CRGB::Blue);
}

testF(FastLEDControllerTest, SegmentScaleMonitor) {
	CRGB leds[130];
	FastLEDController ledController(false);
	fill_solid(leds, 130, CRGB::Black);
	ledController.addLEDs(0, leds, 84);

	fill_solid(leds, 15, CRGB::White);
	fill_solid(leds + 15, 27, CRGB::Red);
	fill_solid(leds + 42, 15, CRGB::Blue);
	fill_solid(leds + 57, 27, CRGB::Green);
	CLP::SegmentScaling segments[] = {{15, 24}, {27, 41}, {15, 24}, {27, 41}};
	CLP::scaleSegments(&ledController, 0, segments, 4);

	assertCRGBArray(leds, 0, 23, CRGB::White);
	assertCRGBArray(leds, 24, 64, CRGB::Red);
	assertCRGBArray(leds, 65, 88, CRGB::Blue);
	assertCRGBArray(leds, 89, 129, CRGB::Green);
}

testF(FastLEDControllerTest, SegmentScaleLongStrip) {
	CRGB leds[41];
	FastLEDController ledController(false);
	fill_solid(leds, 41, CRGB::Black);
	ledController.addLEDs(0, leds, 27);

	fill_solid(leds, 27, CRGB::White);
	CLP::SegmentScaling segments[] = {{27, 41}};
	CLP::scaleSegments(&ledController, 0, segments, 1);

	assertCRGBArray(leds, 0, 40, CRGB::White);
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
