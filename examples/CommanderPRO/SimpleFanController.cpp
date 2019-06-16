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

SimpleFanController::SimpleFanController(uint16_t eEPROMAdress) : eEPROMAdress(eEPROMAdress){}

void SimpleFanController::addFan(uint8_t index, PWMFan* fan)
{
	if (index >= FAN_NUM) {
		return;
	}
	fans[index] = fan;
	fanData[index].detectionType = FAN_MASK_AUTO;
}

uint16_t SimpleFanController::getFanSpeed(uint8_t fan)
{
	return fanData[fan].speed;
}

void SimpleFanController::setFanSpeed(uint8_t fan, uint16_t speed)
{
	fanData[fan].speed = speed;
	fanData[fan].mode = FAN_CONTROL_MODE_FIXED_RPM;
}

uint8_t SimpleFanController::getFanPower(uint8_t fan)
{
	return fanData[fan].power;
}

void SimpleFanController::setFanPower(uint8_t fan, uint8_t percentage)
{
	fanData[fan].power = percentage;
	fanData[fan].mode = FAN_CONTROL_MODE_FIXED_POWER;

	if (fans[fan] != NULL) {
		fans[fan]->setPower(percentage);
	}
}

void SimpleFanController::setFanCurve(uint8_t fan, uint8_t group, FanCurve& fanCurve)
{
	fanData[fan].fanCurve = fanCurve;
	fanData[fan].mode = FAN_CONTROL_MODE_CURVE;
}

void SimpleFanController::setFanExternalTemperature(uint8_t fan, uint16_t temp)
{
	 //TODO
}

void SimpleFanController::setFanForce3PinMode(bool flag)
{
	force3PinMode = flag;
}

uint8_t SimpleFanController::getFanDetectionType(uint8_t fan)
{
	return fanData[fan].detectionType;
}

void SimpleFanController::setFanDetectionType(uint8_t fan, uint8_t type)
{
	fanData[fan].detectionType = type;
}
