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

void CLP::transformLLFanToStrip(FastLEDController* controller, uint8_t channelIndex)
{
	auto& channel = controller->getChannel(channelIndex);
	if (channel.ledMode == CHANNEL_MODE_SOFTWARE_PLAYBACK) {
		auto leds = controller->getLEDs(channelIndex);
		for (uint8_t fanIndex = 0; fanIndex < controller->getLEDCount(channelIndex) / 16; fanIndex++) {
			for (uint8_t ledIndex = 0; ledIndex < 8; ledIndex++) {
				CRGB temp = leds[fanIndex * 16 + ledIndex];
				leds[fanIndex * 16 + ledIndex] = leds[fanIndex * 16 + 15 - ledIndex];
				leds[fanIndex * 16 + 15 - ledIndex] = temp;
			}
		}
	}
}

void CLP::scale(FastLEDController* controller, uint8_t channelIndex, int scaleToSize)
{
	auto leds = controller->getLEDs(channelIndex);
	float scaleFactor = (float)controller->getLEDCount(channelIndex) / scaleToSize;
	for (int ledIndex = scaleToSize - 1; ledIndex >= 0; ledIndex--) {
		leds[ledIndex] = leds[(int)(ledIndex * scaleFactor)];
	}
}

void CLP::repeat(FastLEDController* controller, uint8_t channelIndex, uint8_t times)
{
	auto leds = controller->getLEDs(channelIndex);
	auto count = controller->getLEDCount(channelIndex);
	//skip first iteration, because leds already contains the data at the first position
	for (int i = 1; i < times; i++) {
		memcpy(leds + (count * i), leds, sizeof(CRGB) * count);
	}
}
