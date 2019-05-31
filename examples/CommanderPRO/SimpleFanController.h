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

#include "FanController.h"

// This simple Fan Controller implementation does not implement all features of a Fan Controller.
// It should only demonstrate how to implement your own Fan Controller.
class SimpleFanController : public FanController {
public:
	SimpleFanController(size_t eEPROMAdress);
protected:
	virtual uint16_t getFanSpeed(uint8_t fan) override;
	virtual void setFanSpeed(uint8_t fan, uint16_t speed) override;
	virtual uint8_t getFanPower(uint8_t fan) override;
	virtual void setFanPower(uint8_t fan, uint8_t percentage) override;
	virtual void setFanCurve(uint8_t fan, uint8_t group, const uint16_t* temperatures, const uint16_t* rpms) override;
	virtual void setFanExternalTemperature(uint8_t fan, uint16_t temp) override;
	virtual void setFanForce3PinMode(bool flag) override;
	virtual uint8_t getFanDetectionType(uint8_t fan) override;
	virtual void setFanDetectionType(uint8_t fan, uint8_t type) override;

	bool force3PinMode = false;
	uint8_t detectionType[FAN_NUM] = { FAN_MASK_OFF };
	size_t eEPROMAdress;
};

#endif

