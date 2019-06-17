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
#ifndef _FanController_h
#define _FanController_h

#include "Arduino.h"

#include "CorsairLightingProtocol.h"
#include "IFanController.h"

#define FAN_NUM 6

#define FAN_MASK_DISCONNECTED 0x00
#define FAN_MASK_3PIN 0x01
#define FAN_MASK_4PIN 0x02

#define FAN_DETECTION_TYPE_AUTO 0x00
#define FAN_DETECTION_TYPE_3PIN 0x01
#define FAN_DETECTION_TYPE_4PIN 0x02
#define FAN_DETECTION_TYPE_DISCONNECTED 0x03

#define FAN_FORCE_THREE_PIN_MODE_ON 0x01
#define FAN_FORCE_THREE_PIN_MODE_OFF 0x00

#define FAN_CURVE_POINTS_NUM 6

#define FAN_CURVE_TEMP_GROUP_EXTERNAL 255

struct FanCurve {
	// The temperatures in hundredths of a degree Celsius.
	uint16_t temperatures[FAN_CURVE_POINTS_NUM];
	// The fan speeds in RPM.
	uint16_t rpms[FAN_CURVE_POINTS_NUM];
};

class FanController : public IFanController {
public:
	virtual void handleFanControl(const Command & command, const CorsairLightingProtocolResponse* response) override;
protected:
	// Fan Speed in RPM.
	virtual uint16_t getFanSpeed(uint8_t fan) = 0;
	// Fan Speed in RPM.
	virtual void setFanSpeed(uint8_t fan, uint16_t speed) = 0;
	// percentage 255 mean 100%.
	virtual uint8_t getFanPower(uint8_t fan) = 0;
	// percentage 255 mean 100%.
	virtual void setFanPower(uint8_t fan, uint8_t percentage) = 0;
	virtual void setFanCurve(uint8_t fan, uint8_t group, FanCurve& fanCurve) = 0;
	// The temperature in hundredths of a degree Celsius.
	virtual void setFanExternalTemperature(uint8_t fan, uint16_t temp) = 0;
	virtual void setFanForce3PinMode(bool flag) = 0;
	// One of FAN_MASK_*
	virtual uint8_t getFanDetectionType(uint8_t fan) = 0;
	// One of FAN_MASK_*
	virtual void setFanDetectionType(uint8_t fan, uint8_t type) = 0;
};

#endif
