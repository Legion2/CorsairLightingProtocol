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
#pragma once

#include "Arduino.h"
#include "FastLEDController.h"

namespace CLP {
/**
 * Make it possible to use up to 96 LEDs per channel from iCUE by using the LL Fan option. This function transforms the
 * LL fans layout to a normal strip layout. This function can be combined with scale and repeat function but must be
 * invoked first.
 *
 * @param controller the FastLEDController controlling the LEDs
 * @param channelIndex the index of the channel
 */
void transformLLFanToStrip(FastLEDController* controller, uint8_t channelIndex);
/**
 * Scales a channel's length to a given size, the size can be larger or smaller than the default length given to the
 * FastLEDController::addLEDs function Integer scaling is used, so no interpolation between color values is done and the
 * animation don't look blurry.
 *
 * @param controller the FastLEDController controlling the LEDs
 * @param channelIndex the index of the channel
 * @param scaleToSize the final size after scaling
 */
void scale(FastLEDController* controller, uint8_t channelIndex, int scaleToSize);
/**
 * Repeat a channel's LEDs color to control more LEDs than provided by iCUE.
 *
 * @param controller the FastLEDController controlling the LEDs
 * @param channelIndex the index of the channel
 * @param times the number of time the colors should be repeated
 */
void repeat(FastLEDController* controller, uint8_t channelIndex, uint8_t times);

/**
 * Define the scaling information for a segment of a channel. A segment is a part of a channel that can be scale
 * independently of other segments.
 */
struct SegmentScaling {
	/**
	 * The length of the segment in iCUE for example 10 for the normal LED strips
	 */
	int segmentLength;
	/**
	 * The size to which the segment will be scaled using Integer scaling
	 */
	int scaleToSize;
};
/**
 * Scales a channel's segments to given sizes. This can be used to apply different scaling factors to the different
 * parts of a LED strip. Integer scaling is used.
 *
 * @param controller the FastLEDController controlling the LEDs
 * @param channelIndex the index of the channel you want to scale the segments on
 * @param segments the segments defining the size before and after scaling
 * @param segmentsCount the number of segments
 */
void scaleSegments(FastLEDController* controller, uint8_t channelIndex, const SegmentScaling* const segments,
				   int segmentsCount);

/**
 * Reverse the LEDs of a channel, after this operation, the first LED is the last and the last is the first.
 *
 * @param controller the FastLEDController controlling the LEDs
 * @param channelIndex the index of the channel you want to reverse
 */
void reverse(FastLEDController* controller, uint8_t channelIndex);

/**
 * Simple gamma correction with gamma value 2. This approximation of the gamma correction is sufficient for most LED
 * strips.
 *
 * @param controller the FastLEDController controlling the LEDs
 * @param channelIndex the index of the channel
 */
void gammaCorrection(FastLEDController* controller, uint8_t channelIndex);

/**
 * Increase the brightness of a LED channel when using LS100 and LT100 with iCUE Software lighting, because iCUE only
 * send the RGB value in the range (0 - 127) which is only 50% of max possible brightness. This function doubles the
 * received RGB value. Only use this function with LS100 and LT100.
 *
 * @param controller the FastLEDController controlling the LEDs
 * @param channelIndex the index of the channel
 */
void fixIcueBrightness(FastLEDController* controller, uint8_t channelIndex);
}  // namespace CLP
