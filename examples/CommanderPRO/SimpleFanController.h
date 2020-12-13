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
#include "FanController.h"
#include "PWMFan.h"
#include "TemperatureController.h"

#define FAN_CONTROL_MODE_FIXED_POWER 0
#define FAN_CONTROL_MODE_FIXED_RPM 1
#define FAN_CONTROL_MODE_CURVE 2

struct FanData {
	uint8_t mode = FAN_CONTROL_MODE_FIXED_POWER;
	uint8_t power = 0;
	uint16_t speed = 0;
	FanDetectionType detectionType = FanDetectionType::Disconnected;
	uint8_t tempGroup;
	FanCurve fanCurve;
};

/**
 * This simple Fan Controller implementation does not implement all features of a Fan Controller.
 * It should only demonstrate how to implement your own Fan Controller.
 */
class SimpleFanController : public FanController {
public:
	/**
	 * Fan Controller must use the EEPROM else on startup the fans can't be controlled
	 *
	 * @param temperatureController the TemperatureController used to get the temperature to control the fans
	 * @param updateRate is the time between fan speed updates in ms
	 * @param eEPROMAdress the address where the data is stored in EEPROM
	 */
	SimpleFanController(TemperatureController* temperatureController, uint16_t updateRate, uint16_t eEPROMAdress);
	/**
	 * Add a fan to the Controller.
	 *
	 * @param index the index of the fan
	 * @param fan the fan object
	 */
	void addFan(uint8_t index, PWMFan* fan);
	/**
	 * Update the fan speeds based on the temperature and commands.
	 */
	virtual bool updateFans();

protected:
	virtual uint16_t getFanSpeed(uint8_t fan) override;
	virtual void setFanSpeed(uint8_t fan, uint16_t speed) override;
	virtual uint8_t getFanPower(uint8_t fan) override;
	virtual void setFanPower(uint8_t fan, uint8_t percentage) override;
	virtual void setFanCurve(uint8_t fan, uint8_t group, FanCurve& fanCurve) override;
	virtual void setFanExternalTemperature(uint8_t fan, uint16_t temp) override;
	virtual void setFanForce3PinMode(bool flag) override;
	virtual FanDetectionType getFanDetectionType(uint8_t fan) override;
	virtual void setFanDetectionType(uint8_t fan, FanDetectionType type) override;
	bool load();
	bool save();

	TemperatureController* const temperatureController;
	PWMFan* fans[FAN_NUM] = {nullptr};
	bool force3PinMode = false;
	FanData fanData[FAN_NUM];
	uint16_t externalTemp[FAN_NUM];
	uint16_t updateRate;
	uint16_t eEPROMAdress;
	/**
	 * Indicates that the configuration of the fans has been changed and should be saved.
	 */
	bool triggerSave = false;
	unsigned long lastUpdate = 0;
};
