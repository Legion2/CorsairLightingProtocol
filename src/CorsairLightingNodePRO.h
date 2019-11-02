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
#ifndef _CORSAIRLIGHTINGNODEPRO_h
#define _CORSAIRLIGHTINGNODEPRO_h

#include "Arduino.h"
#include "CorsairLightingFirmware.h" 
#include "FastLEDController.h"
#include "CorsairLightingProtocol.h"
#include "CorsairLightingProtocolHID.h"
#include <FastLED.h>

#if defined(USBCON)

#define CHANNEL_LED_COUNT_DEFAULT 96

class CorsairLightingNodePRO {
public:
	CorsairLightingNodePRO();
	void update();
	FastLEDController* getFastLEDController();
protected:
	CRGB ledsChannel1[CHANNEL_LED_COUNT_DEFAULT];
	CRGB ledsChannel2[CHANNEL_LED_COUNT_DEFAULT];
	CorsairLightingFirmware firmware = corsairLightingNodePROFirmware();
	FastLEDController ledController;
	CorsairLightingProtocol cLP;
	CorsairLightingProtocolHID connectionAdapter;
};

#endif
#endif
