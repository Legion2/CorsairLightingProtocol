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
#include "CorsairLightingProtocolController.h"

#include "LEDController.h"

CorsairLightingProtocolController::CorsairLightingProtocolController(ILEDController* aLEDController,
																	 CorsairLightingFirmware* corsairLightingFirmware)
	: corsairLightingFirmware(corsairLightingFirmware),
	  ledController(aLEDController),
	  temperatureController(nullptr),
	  fanController(nullptr) {}

CorsairLightingProtocolController::CorsairLightingProtocolController(ILEDController* aLEDController,
																	 ITemperatureController* temperatureController,
																	 IFanController* fanController,
																	 CorsairLightingFirmware* corsairLightingFirmware)
	: corsairLightingFirmware(corsairLightingFirmware),
	  ledController(aLEDController),
	  temperatureController(temperatureController),
	  fanController(fanController) {}

void CorsairLightingProtocolController::handleCommand(const Command& command,
													  CorsairLightingProtocolResponse* response) {
	if (command.command < 0x10) {
		corsairLightingFirmware->handleFirmwareCommand(command, response);
	} else if (command.command >= 0x10 && command.command < 0x20) {
		if (temperatureController != nullptr) {
			temperatureController->handleTemperatureControl(command, response);
		} else {
			response->sendError();
		}
	} else if (command.command >= 0x20 && command.command < 0x30) {
		if (fanController != nullptr) {
			fanController->handleFanControl(command, response);
		} else {
			response->sendError();
		}
	} else if (command.command >= 0x30 && command.command < 0x40) {
		ledController->handleLEDControl(command, response);
	} else {
		response->sendError();
	}
}
