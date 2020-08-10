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

void CorsairLightingProtocol::transformLLFanToStrip(FastLEDController* controller, uint8_t channelIndex) {
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
 * Instead of scaling the zero based index we must scale the one based position.
 * So, we first add 1 to the index, scale it, round it up to the next integer
 * and then substract 1 to get the index.
 *
 * @param index the index which should be scaled
 * @param scaleFactor the factor for the scaling
 * @return the scaled index
 */
int scaleIndexAsPosition(int index, const float scaleFactor) { return ceil((index + 1) * scaleFactor) - 1; }

void CorsairLightingProtocol::scale(FastLEDController* controller, uint8_t channelIndex, int scaleToSize) {
	auto leds = controller->getLEDs(channelIndex);
	const float scaleFactor = (float)controller->getLEDCount(channelIndex) / scaleToSize;
	if (scaleFactor < 1.0f) {
		for (int ledIndex = scaleToSize - 1; ledIndex >= 0; ledIndex--) {
			leds[ledIndex] = leds[scaleIndexAsPosition(ledIndex, scaleFactor)];
		}
	} else {
		for (int ledIndex = 0; ledIndex < scaleToSize; ledIndex++) {
			leds[ledIndex] = leds[scaleIndexAsPosition(ledIndex, scaleFactor)];
		}
	}
}

void CorsairLightingProtocol::repeat(FastLEDController* controller, uint8_t channelIndex, uint8_t times) {
	auto leds = controller->getLEDs(channelIndex);
	auto count = controller->getLEDCount(channelIndex);
	// skip first iteration, because LEDs already contains the data at the first position
	for (int i = 1; i < times; i++) {
		memcpy(leds + (count * i), leds, sizeof(CRGB) * count);
	}
}

void CorsairLightingProtocol::scaleSegments(FastLEDController* controller, uint8_t channelIndex,
											const SegmentScaling* const segments, int segmentsCount) {
	auto leds = controller->getLEDs(channelIndex);
	int ledStripIndexAfterScaling = 0;
	int ledStripIndexBeforeScaling = 0;
	int totalLengthAfterScaling = 0;
	SegmentScaling downScaledSegments[segmentsCount];
	// scale down segments and move all segments together so there is space for upscaling
	for (int i = 0; i < segmentsCount; i++) {
		const int segmentLength = segments[i].segmentLength;
		const int scaleToSize = min(segments[i].scaleToSize, segmentLength);
		const float scaleFactor = (float)segmentLength / scaleToSize;

		for (int ledIndex = 0; ledIndex < scaleToSize; ledIndex++) {
			leds[ledStripIndexAfterScaling + ledIndex] =
				leds[ledStripIndexBeforeScaling + scaleIndexAsPosition(ledIndex, scaleFactor)];
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
		const float scaleFactor = (float)downScaledSegments[i].segmentLength / downScaledSegments[i].scaleToSize;
		ledStripIndexAfterScaling -= downScaledSegments[i].scaleToSize;
		ledStripIndexBeforeScaling -= downScaledSegments[i].segmentLength;
		for (int ledIndex = downScaledSegments[i].scaleToSize - 1; ledIndex >= 0; ledIndex--) {
			leds[ledStripIndexAfterScaling + ledIndex] =
				leds[ledStripIndexBeforeScaling + scaleIndexAsPosition(ledIndex, scaleFactor)];
		}
	}
}

void CorsairLightingProtocol::reverse(FastLEDController* controller, uint8_t channelIndex) {
	auto leds = controller->getLEDs(channelIndex);
	auto maxIndex = controller->getLEDCount(channelIndex) - 1;
	for (int ledIndex = 0; ledIndex < maxIndex - ledIndex; ledIndex++) {
		CRGB temp = leds[ledIndex];
		leds[ledIndex] = leds[maxIndex - ledIndex];
		leds[maxIndex - ledIndex] = temp;
	}
}

void CorsairLightingProtocol::gammaCorrection(FastLEDController* controller, uint8_t channelIndex) {
	auto leds = controller->getLEDs(channelIndex);
	auto count = controller->getLEDCount(channelIndex);
	for (int ledIndex = 0; ledIndex < count; ledIndex++) {
		leds[ledIndex].r = dim8_video(leds[ledIndex].r);
		leds[ledIndex].g = dim8_video(leds[ledIndex].g);
		leds[ledIndex].b = dim8_video(leds[ledIndex].b);
	}
}

void CorsairLightingProtocol::fixIcueBrightness(FastLEDController* controller, uint8_t channelIndex) {
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
