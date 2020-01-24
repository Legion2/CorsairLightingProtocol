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
#include "CorsairLightingProtocolConstants.h"
#include "CorsairLightingProtocolResponse.h"

/**
 * The interface of a TemperatureController.
 */
class ITemperatureController {
public:
	/**
	 * Handle a temperature command and send a response.
	 *
	 * @param command the command which should be handled
	 * @param response the callback for the response
	 */
	virtual void handleTemperatureControl(const Command& command, const CorsairLightingProtocolResponse* response) = 0;
};
