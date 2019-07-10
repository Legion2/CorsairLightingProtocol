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
#include "CorsairLightingNodePRO.h"

CorsairLightingNodePRO::CorsairLightingNodePRO(CRGB* ledsChannel1, CRGB* ledsChannel2) : ledController(true), cLP(&ledController, firmware_version), connectionAdapter(&cLP)
{
	ledController.addLeds(0, ledsChannel1);
	ledController.addLeds(1, ledsChannel2);
}

void CorsairLightingNodePRO::update() {
	connectionAdapter.update();

	if (ledController.updateLEDs()) {
		FastLED.show();
	}
}