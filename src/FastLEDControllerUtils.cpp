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
#include "FastLEDControllerUtils.h"

#include <FastLED.h>
#include <math.h>

void CLP::transformLLFanToStrip(FastLEDController* controller, uint8_t channelIndex) {
	auto& channel = controller->getChannel(channelIndex);
	if (channel.mode == ChannelMode::SoftwarePlayback) {
		auto leds = controller->getLEDs(channelIndex);
		auto count = controller->getLEDCount(channelIndex);
		for (uint8_t fanIndex = 0; fanIndex < count / 16; fanIndex++) {
			for (uint8_t ledIndex = 0; ledIndex < 8; ledIndex++) {
				CRGB temp = leds[fanIndex * 16 + ledIndex];
				leds[fanIndex * 16 + ledIndex] = leds[fanIndex * 16 + 15 - ledIndex];
				leds[fanIndex * 16 + 15 - ledIndex] = temp;
			}
		}
	}
}

/**
 * @param index the index which should be scaled
 * @param scaleFactor the factor for the scaling
 * @return the scaled index
 */
inline int scaleIndex(const int index, const float scaleFactor) { return round(index * scaleFactor); }

float scaleFactorOf(const int numberLEDsBefore, const int numberLEDsAfter) {
	return (float)(numberLEDsBefore - 1) / (numberLEDsAfter - 1);
}

void CLP::scale(FastLEDController* controller, uint8_t channelIndex, int scaleToSize) {
	auto leds = controller->getLEDs(channelIndex);
	const float scaleFactor = scaleFactorOf(controller->getLEDCount(channelIndex), scaleToSize);
	if (scaleFactor < 1.0f) {
		for (int ledIndex = scaleToSize - 1; ledIndex >= 0; ledIndex--) {
			leds[ledIndex] = leds[scaleIndex(ledIndex, scaleFactor)];
		}
	} else {
		for (int ledIndex = 0; ledIndex < scaleToSize; ledIndex++) {
			leds[ledIndex] = leds[scaleIndex(ledIndex, scaleFactor)];
		}
	}
}

void CLP::repeat(FastLEDController* controller, uint8_t channelIndex, uint8_t times) {
	auto leds = controller->getLEDs(channelIndex);
	auto count = controller->getLEDCount(channelIndex);
	// skip first iteration, because LEDs already contains the data at the first position
	for (int i = 1; i < times; i++) {
		memcpy(leds + (count * i), leds, sizeof(CRGB) * count);
	}
}

void CLP::scaleSegments(FastLEDController* controller, uint8_t channelIndex, const SegmentScaling* const segments,
						int segmentsCount) {
	auto leds = controller->getLEDs(channelIndex);
	int ledStripIndexAfterScaling = 0;
	int ledStripIndexBeforeScaling = 0;
	int totalLengthAfterScaling = 0;
	SegmentScaling downScaledSegments[segmentsCount];
	// scale down segments and move all segments together so there is space for upscaling
	for (int i = 0; i < segmentsCount; i++) {
		const int segmentLength = segments[i].segmentLength;
		const int scaleToSize = min(segments[i].scaleToSize, segmentLength);
		const float scaleFactor = scaleFactorOf(segmentLength, scaleToSize);

		for (int ledIndex = 0; ledIndex < scaleToSize; ledIndex++) {
			leds[ledStripIndexAfterScaling + ledIndex] =
				leds[ledStripIndexBeforeScaling + scaleIndex(ledIndex, scaleFactor)];
		}
		ledStripIndexAfterScaling += scaleToSize;
		ledStripIndexBeforeScaling += segmentLength;
		downScaledSegments[i].segmentLength = scaleToSize;
		downScaledSegments[i].scaleToSize = segments[i].scaleToSize;
		totalLengthAfterScaling += segments[i].scaleToSize;
	}

	ledStripIndexBeforeScaling = ledStripIndexAfterScaling;
	ledStripIndexAfterScaling = totalLengthAfterScaling;
	// scale up segments beginning with the last segment to not override other segments
	for (int i = segmentsCount - 1; i >= 0; i--) {
		const float scaleFactor = scaleFactorOf(downScaledSegments[i].segmentLength, downScaledSegments[i].scaleToSize);
		ledStripIndexAfterScaling -= downScaledSegments[i].scaleToSize;
		ledStripIndexBeforeScaling -= downScaledSegments[i].segmentLength;
		for (int ledIndex = downScaledSegments[i].scaleToSize - 1; ledIndex >= 0; ledIndex--) {
			leds[ledStripIndexAfterScaling + ledIndex] =
				leds[ledStripIndexBeforeScaling + scaleIndex(ledIndex, scaleFactor)];
		}
	}
}

void CLP::reverse(FastLEDController* controller, uint8_t channelIndex) {
	auto leds = controller->getLEDs(channelIndex);
	auto maxIndex = controller->getLEDCount(channelIndex) - 1;
	for (int ledIndex = 0; ledIndex < maxIndex - ledIndex; ledIndex++) {
		CRGB temp = leds[ledIndex];
		leds[ledIndex] = leds[maxIndex - ledIndex];
		leds[maxIndex - ledIndex] = temp;
	}
}

void CLP::gammaCorrection(FastLEDController* controller, uint8_t channelIndex) {
	auto leds = controller->getLEDs(channelIndex);
	auto count = controller->getLEDCount(channelIndex);
	for (int ledIndex = 0; ledIndex < count; ledIndex++) {
		leds[ledIndex].r = dim8_video(leds[ledIndex].r);
		leds[ledIndex].g = dim8_video(leds[ledIndex].g);
		leds[ledIndex].b = dim8_video(leds[ledIndex].b);
	}
}

void CLP::fixIcueBrightness(FastLEDController* controller, uint8_t channelIndex) {
	auto& channel = controller->getChannel(channelIndex);
	if (channel.mode == ChannelMode::SoftwarePlayback) {
		auto leds = controller->getLEDs(channelIndex);
		auto count = controller->getLEDCount(channelIndex);
		for (int ledIndex = 0; ledIndex < count; ledIndex++) {
			leds[ledIndex].r = leds[ledIndex].r * 2;
			leds[ledIndex].g = leds[ledIndex].g * 2;
			leds[ledIndex].b = leds[ledIndex].b * 2;
		}
	}
}
