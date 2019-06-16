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

SimpleFanController::SimpleFanController(uint16_t updateRate, uint16_t eEPROMAdress) : updateRate(updateRate), eEPROMAdress(eEPROMAdress){}

void SimpleFanController::addFan(uint8_t index, PWMFan* fan)
{
	if (index >= FAN_NUM) {
		return;
	}
	fans[index] = fan;
	fanData[index].detectionType = FAN_MASK_AUTO;
}

bool SimpleFanController::updateFans()
{
	long currentUpdate = millis();
	long lastUpdateNumber = lastUpdate / updateRate;
	long currentUpdateNumber = currentUpdate / updateRate;
	lastUpdate = currentUpdate;
	if (lastUpdateNumber < currentUpdateNumber) {
		for (uint8_t i = 0; i < FAN_NUM; i++) {
			if (fans[i] == NULL || fanData[i].mode != FAN_CONTROL_MODE_CURVE) {
				continue;
			}

			const uint16_t& temp = externalTemp[i];//TODO
			const FanCurve& fanCurve = fanData[i].fanCurve;

			if (temp <= fanCurve.temperatures[0]) {
				fans[i]->setSpeed(fanCurve.rpms[0]);
				continue;
			}
			if (temp > fanCurve.temperatures[FAN_CURVE_POINTS_NUM - 1]) {
				fans[i]->setSpeed(fanCurve.rpms[FAN_CURVE_POINTS_NUM - 1]);
				continue;
			}

			for (uint8_t p = 0; p < FAN_CURVE_POINTS_NUM - 1; p++) {
				if (temp > fanCurve.temperatures[p + 1]) {
					continue;
				}
				uint16_t speed = map(temp, fanCurve.temperatures[p], fanCurve.temperatures[p + 1], fanCurve.rpms[p], fanCurve.rpms[p + 1]);
				fans[i]->setSpeed(speed);
				break;
			}
		}
		return true;
	}
	return false;
}

uint16_t SimpleFanController::getFanSpeed(uint8_t fan)
{
	return fanData[fan].speed;
}

void SimpleFanController::setFanSpeed(uint8_t fan, uint16_t speed)
{
	fanData[fan].speed = speed;
	fanData[fan].mode = FAN_CONTROL_MODE_FIXED_RPM;

	if (fans[fan] != NULL) {
		fans[fan]->setSpeed(speed);
	}
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
	externalTemp[fan] = temp;
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
