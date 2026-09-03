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

#include "CLPUtils.h"

bool CLP::isValidFanControlMode(const FanControlMode mode) {
	return mode == FanControlMode::FixedPower || mode == FanControlMode::FixedRpm || mode == FanControlMode::Curve;
}

CLP::SimpleFanController::SimpleFanController(TemperatureController* temperatureController, uint16_t updateRate,
											  FanControllerStorage* storage)
	: temperatureController(temperatureController), storage(storage), updateRate(updateRate == 0 ? 1 : updateRate) {}

void CLP::SimpleFanController::addFan(uint8_t index, PWMFan* fan) {
	if (index >= FAN_NUM || fan == nullptr) {
		return;
	}
	fans[index] = fan;
}

void CLP::SimpleFanController::begin() {
	if (started) {
		return;
	}
	started = true;
	load();
	for (uint8_t i = 0; i < FAN_NUM; i++) {
		if (fans[i] == nullptr) {
			continue;
		}
		fans[i]->begin();
		FanData& data = fanData[i];
		switch (data.mode) {
			case FanControlMode::FixedPower:
				data.speed = fans[i]->calculateSpeedFromPower(data.power);
				break;
			case FanControlMode::FixedRpm:
				data.power = fans[i]->calculatePowerFromSpeed(data.speed);
				break;
			default:
				break;
		}
		fans[i]->setPower(data.power);
	}
}

bool CLP::SimpleFanController::isValidFanData(const FanData& data) {
	return isValidFanControlMode(data.mode) && isValidFanDetectionType(data.detectionType) &&
		   (data.tempGroup < TEMPERATURE_NUM || data.tempGroup == FAN_CURVE_TEMP_GROUP_EXTERNAL);
}

bool CLP::SimpleFanController::speedFromCurve(uint8_t fan, uint16_t& speed) {
	const FanData& data = fanData[fan];
	uint16_t temp;
	if (data.tempGroup == FAN_CURVE_TEMP_GROUP_EXTERNAL &&
		millis() - externalTempUpdate < FAN_EXTERNAL_TEMPERATURE_TIMEOUT) {
		temp = externalTemp[fan];
	} else if (data.tempGroup < TEMPERATURE_NUM && temperatureController != nullptr &&
			   temperatureController->hasTemperatureSensor(data.tempGroup)) {
		temp = temperatureController->getTemperature(data.tempGroup);
	} else {
		// iCUE stopped sending the temperature or the sensor is gone: use the first local sensor
		bool found = false;
		if (temperatureController != nullptr) {
			for (uint8_t sensor = 0; sensor < TEMPERATURE_NUM; sensor++) {
				if (temperatureController->hasTemperatureSensor(sensor)) {
					temp = temperatureController->getTemperature(sensor);
					found = true;
					break;
				}
			}
		}
		if (!found) {
			return false;
		}
	}

	const FanCurve& curve = data.fanCurve;
	if (temp <= curve.temperatures[0]) {
		speed = curve.rpms[0];
		return true;
	}
	if (temp >= curve.temperatures[FAN_CURVE_POINTS_NUM - 1]) {
		speed = curve.rpms[FAN_CURVE_POINTS_NUM - 1];
		return true;
	}
	for (uint8_t p = 0; p < FAN_CURVE_POINTS_NUM - 1; p++) {
		if (temp > curve.temperatures[p + 1]) {
			continue;
		}
		if (curve.temperatures[p + 1] <= curve.temperatures[p]) {
			speed = curve.rpms[p + 1];
		} else {
			speed = map(temp, curve.temperatures[p], curve.temperatures[p + 1], curve.rpms[p], curve.rpms[p + 1]);
		}
		return true;
	}
	speed = curve.rpms[FAN_CURVE_POINTS_NUM - 1];
	return true;
}

bool CLP::SimpleFanController::updateFans() {
	if (!started) {
		begin();
	}
	const unsigned long currentUpdate = millis();
	const unsigned long lastUpdateNumber = lastUpdate / updateRate;
	const unsigned long currentUpdateNumber = currentUpdate / updateRate;
	lastUpdate = currentUpdate;
	if (lastUpdateNumber >= currentUpdateNumber) {
		return false;
	}
	if (triggerSave) {
		triggerSave = false;
		save();
	}
	for (uint8_t i = 0; i < FAN_NUM; i++) {
		if (fans[i] == nullptr) {
			continue;
		}
		fans[i]->update();
		FanData& data = fanData[i];
		if (data.mode != FanControlMode::Curve) {
			fans[i]->setPower(data.power);
			continue;
		}
		uint16_t speed;
		if (speedFromCurve(i, speed)) {
			data.speed = speed;
			data.power = fans[i]->calculatePowerFromSpeed(speed);
		} else {
			// no temperature at all: full power is the safe choice for a radiator
			data.power = 255;
			data.speed = fans[i]->calculateSpeedFromPower(data.power);
		}
		fans[i]->setPower(data.power);
	}
	return true;
}

uint16_t CLP::SimpleFanController::getFanSpeed(uint8_t fan) {
	if (fans[fan] == nullptr) {
		return 0;
	}
	return fans[fan]->getSpeed();
}

void CLP::SimpleFanController::setFanSpeed(uint8_t fan, uint16_t speed) {
	fanData[fan].speed = speed;
	fanData[fan].mode = FanControlMode::FixedRpm;
	fanData[fan].power = fans[fan] != nullptr ? fans[fan]->calculatePowerFromSpeed(speed) : 0;
	triggerSave = true;
}

uint8_t CLP::SimpleFanController::getFanPower(uint8_t fan) { return fanData[fan].power; }

void CLP::SimpleFanController::setFanPower(uint8_t fan, uint8_t percentage) {
	fanData[fan].power = percentage;
	fanData[fan].mode = FanControlMode::FixedPower;
	fanData[fan].speed = fans[fan] != nullptr ? fans[fan]->calculateSpeedFromPower(percentage) : 0;
	triggerSave = true;
}

void CLP::SimpleFanController::setFanCurve(uint8_t fan, uint8_t group, FanCurve& fanCurve) {
	fanData[fan].fanCurve = fanCurve;
	fanData[fan].tempGroup = group;
	fanData[fan].mode = FanControlMode::Curve;
	triggerSave = true;
}

void CLP::SimpleFanController::setFanExternalTemperature(uint8_t fan, uint16_t temp) {
	externalTemp[fan] = temp;
	externalTempUpdate = millis();
}

void CLP::SimpleFanController::setFanForce3PinMode(bool flag) { force3PinMode = flag; }

FanDetectionType CLP::SimpleFanController::getFanDetectionType(uint8_t fan) {
	if (fans[fan] == nullptr) {
		return FanDetectionType::Disconnected;
	}
	return fanData[fan].detectionType;
}

void CLP::SimpleFanController::setFanDetectionType(uint8_t fan, FanDetectionType type) {
	if (fanData[fan].detectionType != type) {
		fanData[fan].detectionType = type;
		triggerSave = true;
	}
}

FanMask CLP::SimpleFanController::detectFan(uint8_t fan) {
	if (fans[fan] != nullptr && fans[fan]->isConnected()) {
		return FanMask::FourPin;
	}
	return FanMask::Disconnected;
}

bool CLP::SimpleFanController::load() {
	bool allValid = true;
	for (uint8_t i = 0; i < FAN_NUM; i++) {
		if (storage == nullptr || !storage->load(i, fanData[i]) || !isValidFanData(fanData[i])) {
			fanData[i] = FanData();
			allValid = false;
		}
	}
	if (!allValid && storage != nullptr) {
		triggerSave = true;
	}
	return allValid;
}

bool CLP::SimpleFanController::save() {
	if (storage == nullptr) {
		return false;
	}
	CLP_LOG(3, F("Save fan data\r\n"));
	bool saved = true;
	for (uint8_t i = 0; i < FAN_NUM; i++) {
		saved &= storage->save(i, fanData[i]);
	}
	return saved;
}
