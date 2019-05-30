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
#include "SimpleFanController.h"



uint16_t SimpleFanController::getFanSpeed(uint8_t fan)
{
	return random16();
}

void SimpleFanController::setFanSpeed(uint8_t fan, uint16_t speed)
{
}

uint8_t SimpleFanController::getFanPower(uint8_t fan)
{
	return 0;
}

void SimpleFanController::setFanPower(uint8_t fan, uint8_t percentage)
{
}

void SimpleFanController::setFanCurve(uint8_t fan, uint8_t group, const uint16_t* temperatures, const uint16_t* rpms)
{
}

void SimpleFanController::setFanExternalTemperature(uint8_t fan, uint16_t temp)
{
}

void SimpleFanController::setFanForce3PinMode(bool flag)
{
}

uint8_t SimpleFanController::getFanDetectionType(uint8_t fan)
{
	return detectionType[fan];
}

void SimpleFanController::setFanDetectionType(uint8_t fan, uint8_t type)
{
	detectionType[fan] = type;
}
