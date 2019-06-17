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
#ifndef _SimpleFanController_h
#define _SimpleFanController_h

#include "Arduino.h"
#include "FanController.h"
#include "TemperatureController.h"
#include "PWMFan.h"

#define FAN_CONTROL_MODE_FIXED_POWER 0
#define FAN_CONTROL_MODE_FIXED_RPM 1
#define FAN_CONTROL_MODE_CURVE 2

struct FanData {
	uint8_t mode = FAN_CONTROL_MODE_FIXED_POWER;
	uint8_t power = 0;
	uint16_t speed = 0;
	uint8_t detectionType = FAN_DETECTION_TYPE_DISCONNECTED;
	uint8_t tempGroup;
	FanCurve fanCurve;
};

// This simple Fan Controller implementation does not implement all features of a Fan Controller.
// It should only demonstrate how to implement your own Fan Controller.
class SimpleFanController : public FanController {
public:
	// Fan Contorller must use the EEPROM else on startup the fans can't be controlled
	// updateRate it the time between fan speed updates in ms
	SimpleFanController(TemperatureController* temperatureController, uint16_t updateRate, uint16_t eEPROMAdress);
	void addFan(uint8_t index, PWMFan* fan);
	virtual bool updateFans();
protected:
	virtual uint16_t getFanSpeed(uint8_t fan) override;
	virtual void setFanSpeed(uint8_t fan, uint16_t speed) override;
	virtual uint8_t getFanPower(uint8_t fan) override;
	virtual void setFanPower(uint8_t fan, uint8_t percentage) override;
	virtual void setFanCurve(uint8_t fan, uint8_t group, FanCurve& fanCurve) override;
	virtual void setFanExternalTemperature(uint8_t fan, uint16_t temp) override;
	virtual void setFanForce3PinMode(bool flag) override;
	virtual uint8_t getFanDetectionType(uint8_t fan) override;
	virtual void setFanDetectionType(uint8_t fan, uint8_t type) override;
	bool load();
	bool save();

	TemperatureController* const temperatureController;
	PWMFan* fans[FAN_NUM] = { NULL };
	bool force3PinMode = false;
	FanData fanData[FAN_NUM];
	uint16_t externalTemp[FAN_NUM];
	uint16_t updateRate;
	uint16_t eEPROMAdress;
	bool trigger_save = false;
	long lastUpdate = 0;
};

#endif

