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

namespace CLP
{
	//Make it possible to use up to 96 leds per channel from iCUE by using the LL Fan option. This function transforms the LL fans layout to a normal strip layout.
	//This function can be combined with scale and repeat function but must be invoked first.
	void transformLLFanToStrip(FastLEDController* controller, uint8_t channelIndex);
	//Scales a channel's length to a given size, the size can be larger or smaller than the default length given to the FastLEDController::addLEDs function
	//Integer scaling is used, so no interpolation between color values is done and the animation don't look blurry.
	void scale(FastLEDController* controller, uint8_t channelIndex, int scaleToSize);
	//Repeat a channel's leds color to control more leds than provided by iCUE.
	void repeat(FastLEDController* controller, uint8_t channelIndex, uint8_t times);

	/**
	 * Define the scaling information for a segment of a channel. A segment is an part of a channel that you want to scale independent of other segments.
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
	 * Scales a channel's segment to a given size. This can be used to apply different scaling factors to the different parts of a LED strip.
	 * Integer scaling is used.
	 * 
	 * @param controller the FastLEDController controlling the LEDs
	 * @param channelIndex the index of the channel you want to scale the segments on
	 * @param segments the segments defining the size before and after scaling
	 * @param segmentsCount the number of segments
	 */
	void scaleSegments(FastLEDController* controller, uint8_t channelIndex, const SegmentScaling* const segments, int segmentsCount);

	/**
	 * Reverse the LEDs of a channel, after this operation, the first led is the last and the last is the first.
	 *
	 * @param controller the FastLEDController controlling the LEDs
	 * @param channelIndex the index of the channel you want to reverse
	 */
	void reverse(FastLEDController* controller, uint8_t channelIndex);
}
